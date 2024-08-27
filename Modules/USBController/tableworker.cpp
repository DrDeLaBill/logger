#include "tableworker.h"

#include "fsm_gc.h"

#include "app.h"
#include "settings.h"

#include "usbdreport.h"
#include "usbhreport.h"


fsm::FiniteStateMachine<typename TableWorker::fsm_table> TableWorker::fsm;
uint32_t TableWorker::curr_key = 0;
uint32_t TableWorker::counter = 0;
unsigned TableWorker::errors_count = 0;
USBCStatus TableWorker::result = USBC_RES_OK;
uint8_t TableWorker::index = 0;
utl::Timer TableWorker::timer(COM_DELAY_MS);
std::unordered_map<uint32_t, gtuple> TableWorker::table = {
    {GP_KEY_STR("dv_type"),               {reinterpret_cast<uint8_t*>(&settings.dv_type),               sizeof(settings.dv_type)}},
    {GP_KEY_STR("sw_id"),                 {reinterpret_cast<uint8_t*>(&settings.sw_id),                 sizeof(settings.sw_id)}},
    {GP_KEY_STR("fw_id"),                 {reinterpret_cast<uint8_t*>(&settings.fw_id),                 sizeof(settings.fw_id)}},
    {GP_KEY_STR("record_period"),         {reinterpret_cast<uint8_t*>(&settings.record_period),         sizeof(settings.record_period)}},
    {GP_KEY_STR("send_period"),           {reinterpret_cast<uint8_t*>(&settings.send_period),           sizeof(settings.send_period)}},
    {GP_KEY_STR("record_id"),             {reinterpret_cast<uint8_t*>(&settings.record_id),             sizeof(settings.record_id)}},
    {GP_KEY_STR("modbus1_status"),        {reinterpret_cast<uint8_t*>(&settings.modbus1_status),        sizeof(settings.modbus1_status[0]),       __arr_len(settings.modbus1_status),     modbus1_index}},
    {GP_KEY_STR("modbus1_value_reg"),     {reinterpret_cast<uint8_t*>(&settings.modbus1_value_reg),     sizeof(settings.modbus1_value_reg[0]),    __arr_len(settings.modbus1_value_reg),  modbus1_index}},
    {GP_KEY_STR("modbus1_id_reg"),        {reinterpret_cast<uint8_t*>(&settings.modbus1_id_reg),        sizeof(settings.modbus1_id_reg[0]),       __arr_len(settings.modbus1_id_reg),     modbus1_index}},
    {GP_KEY_STR("_1wire_address"),        {reinterpret_cast<uint8_t*>(&settings._1wire_address),        sizeof(settings._1wire_address[0]),       __arr_len(settings._1wire_address),     _1wire_index}},
    {GP_KEY_STR("mb1_last_id"),           {reinterpret_cast<uint8_t*>(&app_info.mb1_last_id),           sizeof(app_info.mb1_last_id)}},
    {GP_KEY_STR("mb1_new_id"),            {reinterpret_cast<uint8_t*>(&app_info.mb1_new_id),            sizeof(app_info.mb1_new_id)}},
    {GP_KEY_STR("need_mb1_id_update"),    {reinterpret_cast<uint8_t*>(&app_info.need_mb1_id_update),    sizeof(app_info.need_mb1_id_update)}},
    {GP_KEY_STR("time"),                  {reinterpret_cast<uint8_t*>(&app_info.time),                  sizeof(app_info.time)}},
    {GP_KEY_STR("need_registrate_1wire"), {reinterpret_cast<uint8_t*>(&app_info.need_registrate_1wire), sizeof(app_info.need_registrate_1wire)}},
    {GP_KEY_STR("modbus1_last_value"),    {reinterpret_cast<uint8_t*>(&app_info.modbus1_last_value),    sizeof(app_info.modbus1_last_value[0]),   __arr_len(app_info.modbus1_last_value), modbus1_index}},
    {GP_KEY_STR("_1wire_last_value"),     {reinterpret_cast<uint8_t*>(&app_info._1wire_last_value),     sizeof(app_info._1wire_last_value[0]),    __arr_len(app_info._1wire_last_value),  _1wire_index}},
    {GP_KEY_STR("_1wire_registrate"),     {reinterpret_cast<uint8_t*>(&app_info._1wire_registrate),     sizeof(app_info._1wire_registrate[0]),    __arr_len(app_info._1wire_registrate),  _1wire_index}},
};
gprotocol TableWorker::protocol(table);
std::unordered_map<uint32_t, std::vector<uint32_t>> TableWorker::get_hashes;


USBCStatus TableWorker::usbSessionProccess() const
{
    result = USBC_RES_OK;

    do {
        fsm.proccess();
    } while (!fsm.is_state(idle_s{}));

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
    uint32_t hash = 0;

    for(unsigned i = 0; i < 8; i++) {
        hash += (value >> (i * BITS_IN_BYTE));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

void TableWorker::_idle_s::operator()(void) const {}

void TableWorker::_update_s::operator()(void) const
{
    result = USBC_RES_OK;

    if (errors_count > ERRORS_COUNT_MAX) {
        fsm.push_event(error_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (counter >= table.size()) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    try {
        protocol.master_send(0, curr_key, index);
    } catch (...) {
        result = USBC_RES_ERROR;
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
    result = USBC_RES_OK;

    if (errors_count > ERRORS_COUNT_MAX) {
        fsm.push_event(error_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (counter >= table.size()) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    try {
        protocol.master_send(1, curr_key, index);
    } catch (...) {
        result = USBC_RES_ERROR;
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


void TableWorker::iterate_upgrade_a::operator()(void) const
{
    index++;

    while (counter < table.size()) {

        while (index < table.at(curr_key).length() &&
               get_hashes[curr_key].at(index) == value_hash(protocol.get(curr_key, index))
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

        if (get_hashes[curr_key].at(index) != value_hash(protocol.get(curr_key, index))) {
            break;
        }
    }

    timer.start();
}


void TableWorker::end_a::operator()(void) const
{
    fsm.clear_events();

    for (auto& [ key, item ] : table) {
        get_hashes[key].clear();
        for (unsigned i = 0; i < item.length(); i++) {
            get_hashes[key].push_back(value_hash(protocol.get(key, i)));
        }
    }
}


void TableWorker::count_error_a::operator()(void) const
{
    fsm.clear_events();
    errors_count++;
    timer.start();
}
