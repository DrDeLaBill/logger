#ifndef COMTABLEWORKER_H
#define COMTABLEWORKER_H


#include "gtime.h"

#include "Timer.h"
#include "FiniteStateMachine.h"

#include "COMTable.h"
#include "usbdreport.h"
#include "comservice.h"
#include "usbcstatus.h"
#include "COMController.h"


template<class Table, uint16_t START_ID>
struct COMTableWorker
{
    static_assert(
        !utl::empty(typename utl::typelist_t<Table>::RESULT{}),
        "COMTableWorker must not be empty"
    );

protected:
    static constexpr unsigned ERRORS_COUNT_MAX = 5;

    static COMController<Table> hid_table;

    static uint16_t characteristic_id;
    static uint16_t stop_id;
    static unsigned errors_count;
    static USBCStatus result;
    static utl::Timer timer;
    static uint8_t index;

    // Events:
    FSM_CREATE_EVENT(ready_e,   0);
    FSM_CREATE_EVENT(success_e, 0);
    FSM_CREATE_EVENT(update_e,  0);
    FSM_CREATE_EVENT(upgrade_e, 0);
    FSM_CREATE_EVENT(end_e,     1);
    FSM_CREATE_EVENT(timeout_e, 2);
    FSM_CREATE_EVENT(error_e,   3);

private:
    // States:
    struct _idle_s    { void operator()(void) const; };
    struct _update_s  { void operator()(void) const; };
    struct _upgrade_s { void operator()(void) const; };

    FSM_CREATE_STATE(idle_s,    _idle_s);
    FSM_CREATE_STATE(update_s,  _update_s);
    FSM_CREATE_STATE(upgrade_s, _upgrade_s);

    // Actions:
    struct init_characteristics_a    { void operator()(void) const; };
    struct iterate_characteristics_a { void operator()(void) const; };
    struct start_idle_a              { void operator()(void) const; };
    struct count_error_a             { void operator()(void) const; };

    using fsm_table = fsm::TransitionTable<
        fsm::Transition<idle_s,    update_e,  update_s,  init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    upgrade_e, upgrade_s, init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  success_e, update_s,  iterate_characteristics_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  timeout_e, update_s,  count_error_a,             fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  end_e,     idle_s,    start_idle_a,              fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  error_e,   idle_s,    init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, success_e, upgrade_s, iterate_characteristics_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, timeout_e, upgrade_s, count_error_a,             fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, end_e,     idle_s,    start_idle_a,              fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, error_e,   idle_s,    init_characteristics_a,    fsm::Guard::NO_GUARD>
    >;

    USBCStatus usbSessionProccess() const
    {
        result = USBC_RES_OK;

        COMService::init(COM_PRODUCT_ID);

        do {
            fsm.proccess();
        } while (!fsm.is_state(idle_s{}));

        COMService::deinit();

        return result;
    }

protected:
    static fsm::FiniteStateMachine<fsm_table> fsm;

public:
    static constexpr uint16_t maxID()
    {
        return START_ID + COMController<Table>::count() - 1;
    }

    USBCStatus load() const
    {
        characteristic_id = START_ID;
        stop_id           = maxID();

        fsm.push_event(update_e{});

        return usbSessionProccess();
    }

    USBCStatus loadCharacteristic(const uint16_t need_id) const
    {
        characteristic_id = need_id;
        stop_id           = need_id;

        fsm.push_event(update_e{});

        return usbSessionProccess();
    }

    USBCStatus save() const
    {
        characteristic_id = START_ID;
        stop_id           = maxID();

        fsm.push_event(upgrade_e{});

        return usbSessionProccess();
    }
};

template<class Table, uint16_t START_ID>
fsm::FiniteStateMachine<typename COMTableWorker<Table, START_ID>::fsm_table> COMTableWorker<Table, START_ID>::fsm;

template<class Table, uint16_t START_ID>
COMController<Table> COMTableWorker<Table, START_ID>::hid_table(START_ID);

template<class Table, uint16_t START_ID>
uint16_t COMTableWorker<Table, START_ID>::characteristic_id = START_ID;

template<class Table, uint16_t START_ID>
uint16_t COMTableWorker<Table, START_ID>::stop_id = COMTableWorker<Table, START_ID>::maxID();

template<class Table, uint16_t START_ID>
unsigned COMTableWorker<Table, START_ID>::errors_count = 0;

template<class Table, uint16_t START_ID>
USBCStatus COMTableWorker<Table, START_ID>::result = USBC_RES_OK;

template<class Table, uint16_t START_ID>
uint8_t COMTableWorker<Table, START_ID>::index = 0;

template<class Table, uint16_t START_ID>
utl::Timer COMTableWorker<Table, START_ID>::timer(COM_DELAY_MS);

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::_idle_s::operator()(void) const {}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::_update_s::operator()(void) const
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

    if (characteristic_id > maxID() ||
        characteristic_id > stop_id
    ) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    report_pack_t report = {};
    report.characteristic_id = COM_GETTER_ID;
    report.index = index;
    std::shared_ptr<uint8_t[]> data = utl::serialize<uint16_t>(&characteristic_id);
    com_report_set_data(&report, data.get(), sizeof(uint16_t));

    try {
        COMService::sendReport(report);
    } catch (...) {
        result = USBC_RES_ERROR;
        fsm.push_event(timeout_e{});
        return;
    }

    report_pack_t& response = USBDReport::getReport();
    hid_table.setValue(response.characteristic_id, response.data, response.index);
    index = response.index;

    fsm.push_event(success_e{});
}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::_upgrade_s::operator()(void) const
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

    if (characteristic_id > maxID()) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    if (!hid_table.isUpdated(characteristic_id, index)) {
        fsm.push_event(success_e{});
        return;
    }

    report_pack_t report = {};
    report.characteristic_id = characteristic_id;
    report.index = index;
    uint8_t data[sizeof(uint32_t)] = {};
    hid_table.getValue(characteristic_id, data, index);
    hid_report_set_data(&report, data, sizeof(uint32_t));

    try {
        COMService::sendReport(report);
    } catch (...) {
        result = USBC_RES_ERROR;
        fsm.push_event(timeout_e{});
        return;
    }

    report_pack_t& response = USBDReport::getReport();
    hid_table.setValue(response.characteristic_id, response.data, response.index);
    index = response.index;
    hid_table.resetUpdated(characteristic_id, index);

    fsm.push_event(success_e{});
}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::init_characteristics_a::operator()(void) const
{
    errors_count = 0;
    index = 0;

    fsm.clear_events();

    timer.start();
}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::iterate_characteristics_a::operator()(void) const
{
    index++;

    if (index >= hid_table.characteristicLength(characteristic_id)) {
        characteristic_id++;
        index = 0;
        errors_count = 0;
    }

    timer.start();
}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::start_idle_a::operator()(void) const
{
    fsm.clear_events();
}

template<class Table, uint16_t START_ID>
void COMTableWorker<Table, START_ID>::count_error_a::operator()(void) const
{
    fsm.clear_events();
    errors_count++;
    timer.start();
}

#endif // COMTableWorker_H
