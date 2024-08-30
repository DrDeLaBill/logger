#include "tableworker.h"

#include "fsm_gc.h"

#include "app.h"
#include "settings.h"

#include "usbdreport.h"
#include "usbhreport.h"


fsm::FiniteStateMachine<typename TableWorker::fsm_table> TableWorker::fsm;
bool TableWorker::firstLoad = true;
uint32_t TableWorker::curr_key = 0;
uint32_t TableWorker::counter = 0;
unsigned TableWorker::errors_count = 0;
USBCStatus TableWorker::result = USBC_WAIT;
uint8_t TableWorker::index = 0;
utl::Timer TableWorker::timer(COM_DELAY_MS);
std::unordered_map<uint32_t, gtuple> TableWorker::table = {
    {GP_KEY_STR("dv_type"),               {(uint8_t*)(&settings_be.dv_type),               sizeof(settings_be.dv_type)}},
    {GP_KEY_STR("sw_id"),                 {(uint8_t*)(&settings_be.sw_id),                 sizeof(settings_be.sw_id)}},
    {GP_KEY_STR("fw_id"),                 {(uint8_t*)(&settings_be.fw_id),                 sizeof(settings_be.fw_id)}},
    {GP_KEY_STR("record_period"),         {(uint8_t*)(&settings_be.record_period),         sizeof(settings_be.record_period)}},
    {GP_KEY_STR("send_period"),           {(uint8_t*)(&settings_be.send_period),           sizeof(settings_be.send_period)}},
    {GP_KEY_STR("record_id"),             {(uint8_t*)(&settings_be.record_id),             sizeof(settings_be.record_id)}},
    {GP_KEY_STR("modbus1_status"),        {(uint8_t*)(&settings_be.modbus1_status),        sizeof(settings_be.modbus1_status[0]),       __arr_len(settings_be.modbus1_status),     modbus1_index_be}},
    {GP_KEY_STR("modbus1_value_reg"),     {(uint8_t*)(&settings_be.modbus1_value_reg),     sizeof(settings_be.modbus1_value_reg[0]),    __arr_len(settings_be.modbus1_value_reg),  modbus1_index_be}},
    {GP_KEY_STR("modbus1_id_reg"),        {(uint8_t*)(&settings_be.modbus1_id_reg),        sizeof(settings_be.modbus1_id_reg[0]),       __arr_len(settings_be.modbus1_id_reg),     modbus1_index_be}},
    {GP_KEY_STR("_1wire_address"),        {(uint8_t*)(&settings_be._1wire_address),        sizeof(settings_be._1wire_address[0]),       __arr_len(settings_be._1wire_address),     _1wire_index_be}},
    {GP_KEY_STR("mb1_last_id"),           {(uint8_t*)(&app_info_be.mb1_last_id),           sizeof(app_info_be.mb1_last_id)}},
    {GP_KEY_STR("mb1_new_id"),            {(uint8_t*)(&app_info_be.mb1_new_id),            sizeof(app_info_be.mb1_new_id)}},
    {GP_KEY_STR("need_mb1_id_update"),    {(uint8_t*)(&app_info_be.need_mb1_id_update),    sizeof(app_info_be.need_mb1_id_update)}},
    {GP_KEY_STR("time"),                  {(uint8_t*)(&app_info_be.time),                  sizeof(app_info_be.time)}},
    {GP_KEY_STR("need_registrate_1wire"), {(uint8_t*)(&app_info_be.need_registrate_1wire), sizeof(app_info_be.need_registrate_1wire)}},
    {GP_KEY_STR("modbus1_last_value"),    {(uint8_t*)(&app_info_be.modbus1_last_value),    sizeof(app_info_be.modbus1_last_value[0]),   __arr_len(app_info_be.modbus1_last_value), modbus1_index_be}},
    {GP_KEY_STR("_1wire_last_value"),     {(uint8_t*)(&app_info_be._1wire_last_value),     sizeof(app_info_be._1wire_last_value[0]),    __arr_len(app_info_be._1wire_last_value),  _1wire_index_be}},
    {GP_KEY_STR("_1wire_registrate"),     {(uint8_t*)(&app_info_be._1wire_registrate),     sizeof(app_info_be._1wire_registrate[0]),    __arr_len(app_info_be._1wire_registrate),  _1wire_index_be}},
};
gprotocol TableWorker::protocol(table);
std::unordered_map<uint32_t, std::vector<uint32_t>> TableWorker::get_hashes;
std::unordered_map<uint32_t, std::vector<uint32_t>> TableWorker::set_hashes;


USBCStatus TableWorker::usbSessionProccess() const
{
    result = USBC_IN_PROGRESS;

    do {
        fsm.proccess();
    } while (result == USBC_IN_PROGRESS);

    return result;
}

USBCStatus TableWorker::load() const
{
    fsm.push_event(update_e{});

    return usbSessionProccess();
}

USBCStatus TableWorker::save()
{
    fsm.push_event(upgrade_e{});

    return usbSessionProccess();
}

uint32_t TableWorker::value_hash(gprotocol::type_t value)
{
    return util_hash((uint8_t*)&value, sizeof(value));
}

void TableWorker::_init_s::operator()(void) const
{
    for (const auto& [ key, item ] : table) {
        get_hashes[key].clear();
        set_hashes[key].clear();
        for (unsigned i = 0; i < item.length(); i++) {
            get_hashes[key].push_back(value_hash(protocol.get(key, i)));
            set_hashes[key].push_back(value_hash(protocol.get(key, i)));
        }
    }

    fsm.push_event(success_e{});
}

void TableWorker::_idle_s::operator()(void) const {}

void TableWorker::_update_s::operator()(void) const
{
    if (errors_count > ERRORS_COUNT_MAX) {
        fsm.push_event(error_e{});
        return;
    }

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        return;
    }

    if (counter >= table.size()) {
        fsm.push_event(end_e{});
        return;
    }

    try {
        protocol.master_send(0, curr_key, index);
    } catch (...) {
        fsm.push_event(timeout_e{});
        return;
    }

    pack_t& response = USBDReport::getReport();
    protocol.master_recieve(&response);
    index = response.index;

    fsm.push_event(success_e{});
}


void TableWorker::_upgrade_s::operator()(void) const
{
    if (errors_count > ERRORS_COUNT_MAX) {
        fsm.push_event(error_e{});
        return;
    }

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        return;
    }

    if (counter >= table.size()) {
        fsm.push_event(end_e{});
        return;
    }

    try {
        protocol.master_send(1, curr_key, index);
    } catch (...) {
        fsm.push_event(timeout_e{});
        return;
    }

    pack_t& response = USBDReport::getReport();
    protocol.master_recieve(&response);
    index = response.index;

    fsm.push_event(success_e{});
}


void TableWorker::init_a::operator()(void) const
{
    curr_key = table.begin()->first;
    errors_count = 0;
    counter = 0;
    index = 0;

    fsm.clear_events();

    timer.start();
}


void TableWorker::iterate_update_a::operator()(void) const
{
    index++;

    if (index >= table.at(curr_key).length()) {
        curr_key = 0;
        uint32_t it = 0;
        for (const auto & [ key, value ] : table) {
            if (it == counter) {
                curr_key = key;
                counter++;
                break;
            }
            it++;
        }
        index = 0;
        errors_count = 0;
    }

    timer.start();
}


void TableWorker::init_upgrade_a::operator()(void) const
{
    curr_key = table.begin()->first;
    errors_count = 0;
    counter = 0;
    index = 0;

    fsm.clear_events();

    while (counter < table.size()) {
        while (index < table.at(curr_key).length() &&
               set_hashes[curr_key].at(index) == value_hash(protocol.get(curr_key, index))
        ) {
            index++;
        }

        if (index >= table.at(curr_key).length()) {
            curr_key = 0;
            uint32_t it = 0;

            for (const auto & [ key, value ] : table) {
                if (it == counter) {
                    curr_key = key;
                    counter++;
                    break;
                }
                it++;
            }

            index = 0;
            errors_count = 0;
        }

        printPretty(
            "%010u -> %u - %u (value=%llu)\n",
            curr_key,
            set_hashes[curr_key].at(index),
            value_hash(protocol.get(curr_key, index)),
            protocol.get(curr_key, index)
        );
        if (set_hashes[curr_key].at(index) != value_hash(protocol.get(curr_key, index))) {
            break;
        }
    }

    timer.start();
}


void TableWorker::iterate_upgrade_a::operator()(void) const
{
    errors_count = 0;
    index++;

    while (counter < table.size()) {
        while (index < table.at(curr_key).length() &&
               set_hashes[curr_key].at(index) == value_hash(protocol.get(curr_key, index))
        ) {
            index++;
        }

        if (index >= table.at(curr_key).length()) {
            curr_key = 0;
            uint32_t it = 0;

            for (const auto & [ key, value ] : table) {
                if (it == counter) {
                    curr_key = key;
                    counter++;
                    break;
                }
                it++;
            }

            index = 0;
            errors_count = 0;
        }

        printPretty(
            "%010u -> %u - %u (value=%llu)\n",
            curr_key,
            set_hashes[curr_key].at(index),
            value_hash(protocol.get(curr_key, index)),
            protocol.get(curr_key, index)
        );
        if (set_hashes[curr_key].at(index) != value_hash(protocol.get(curr_key, index))) {
            break;
        }
    }

    timer.start();
}


void TableWorker::end_update_a::operator()(void) const
{
    fsm.clear_events();

    for (const auto& [ key, item ] : table) {
        get_hashes[key].clear();
        if (firstLoad) {
            set_hashes[key].clear();
        }
        for (unsigned i = 0; i < item.length(); i++) {
            get_hashes[key].push_back(value_hash(protocol.get(key, i)));
            if (firstLoad) {
                set_hashes[key].push_back(value_hash(protocol.get(key, i)));
            }
        }
    }

    firstLoad = false;

    result = USBC_RES_DONE;
}


void TableWorker::end_upgrade_a::operator()(void) const
{
    fsm.clear_events();

    firstLoad = true;

    result = USBC_RES_DONE;
}


void TableWorker::count_error_a::operator()(void) const
{
    fsm.clear_events();
    errors_count++;
    timer.start();
}


void TableWorker::error_a::operator()(void) const
{
    fsm.clear_events();

    firstLoad = true;

    result = USBC_RES_ERROR;
}
