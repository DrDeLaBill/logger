#ifndef HIDTABLESETTINGS_H
#define HIDTABLESETTINGS_H


#include "Timer.h"
#include "FiniteStateMachine.h"

#include "HIDTable.h"
#include "usbdreport.h"
#include "hidservice.h"
#include "usbcstatus.h"
#include "HIDController.h"


template<class Table>
struct HIDTableSettings
{
    static_assert(
        !utl::empty(typename utl::typelist_t<Table>::RESULT{}),
        "HIDTableSettings must not be empty"
    );

protected:
    static constexpr unsigned ERRORS_COUNT_MAX = 5;

    static HIDController<Table> hid_table;

    static uint16_t characteristic_id;
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
    struct _init_s    { void operator()(void) const; };
    struct _idle_s    { void operator()(void) const; };
    struct _update_s  { void operator()(void) const; };
    struct _upgrade_s { void operator()(void) const; };

    FSM_CREATE_STATE(init_s,    _init_s);
    FSM_CREATE_STATE(idle_s,    _idle_s);
    FSM_CREATE_STATE(update_s,  _update_s);
    FSM_CREATE_STATE(upgrade_s, _upgrade_s);

    // Actions:
    struct init_characteristics_a    { void operator()(void) const; };
    struct iterate_characteristics_a { void operator()(void) const; };
    struct start_init_a              { void operator()(void) const; };
    struct start_idle_a              { void operator()(void) const; };
    struct count_error_a             { void operator()(void) const; };
    struct reset_usb_a               { void operator()(void) const; };

    using fsm_table = fsm::TransitionTable<
        fsm::Transition<init_s,    success_e, update_s,  init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    update_e,  update_s,  init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    upgrade_e, upgrade_s, init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    success_e, init_s,    start_init_a,              fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  success_e, update_s,  iterate_characteristics_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  timeout_e, update_s,  count_error_a,             fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  end_e,     idle_s,    start_idle_a,              fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  error_e,   init_s,    reset_usb_a,               fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, success_e, upgrade_s, iterate_characteristics_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, timeout_e, upgrade_s, count_error_a,             fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, end_e,     idle_s,    start_idle_a,              fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, error_e,   init_s,    reset_usb_a,               fsm::Guard::NO_GUARD>
    >;

    USBCStatus usbSessionProccess() const
    {
        result = USBC_RES_OK;

        HIDService::init();

        do {
            if (errors_count > ERRORS_COUNT_MAX) {
                result = USBC_RES_ERROR;
                fsm.push_event(error_e{});
            }
            fsm.proccess();
        } while (!fsm.is_state(idle_s{}));

        HIDService::deinit();

        return result;
    }

protected:
    static fsm::FiniteStateMachine<fsm_table> fsm;

public:
    USBCStatus load() const
    {
        fsm.push_event(update_e{});

        return usbSessionProccess();
    }

    USBCStatus save() const
    {
        fsm.push_event(upgrade_e{});

        return usbSessionProccess();
    }
};

template<class Table>
fsm::FiniteStateMachine<typename HIDTableSettings<Table>::fsm_table> HIDTableSettings<Table>::fsm;

template<class Table>
HIDController<Table> HIDTableSettings<Table>::hid_table;

template<class Table>
uint16_t HIDTableSettings<Table>::characteristic_id = HID_FIRST_KEY;

template<class Table>
unsigned HIDTableSettings<Table>::errors_count = 0;

template<class Table>
USBCStatus HIDTableSettings<Table>::result = USBC_RES_OK;

template<class Table>
uint8_t HIDTableSettings<Table>::index = 0;

template<class Table>
utl::Timer HIDTableSettings<Table>::timer(HID_DELAY_MS);

template<class Table>
void HIDTableSettings<Table>::_init_s::operator()(void) const
{
    fsm.push_event(success_e{});
    result = USBC_RES_OK;
}

template<class Table>
void HIDTableSettings<Table>::_idle_s::operator()(void) const
{

}

template<class Table>
void HIDTableSettings<Table>::_update_s::operator()(void) const
{
    result = USBC_RES_OK;

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (characteristic_id > hid_table.maxKey()) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    report_pack_t report = {};
    report.characteristic_id = HID_GETTER_ID;
    report.index = index;
    std::shared_ptr<uint8_t[]> data = utl::serialize<uint16_t>(&characteristic_id);
    hid_report_set_data(&report, data.get(), sizeof(uint16_t));

    try {
        HIDService::sendReport(HID_VENDOR_ID, HID_PRODUCT_ID, report);
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

template<class Table>
void HIDTableSettings<Table>::_upgrade_s::operator()(void) const
{
    result = USBC_RES_OK;

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        result = USBC_RES_ERROR;
        return;
    }

    if (characteristic_id > hid_table.maxKey()) {
        fsm.push_event(end_e{});
        result = USBC_RES_DONE;
        return;
    }

    if (!hid_table.isUpdated(characteristic_id)) {
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
        HIDService::sendReport(HID_VENDOR_ID, HID_PRODUCT_ID, report);
    } catch (...) {
        result = USBC_RES_ERROR;
        fsm.push_event(timeout_e{});
        return;
    }

    report_pack_t& response = USBDReport::getReport();
    hid_table.setValue(response.characteristic_id, response.data, response.index);
    index = response.index;
    hid_table.resetUpdated(characteristic_id);

    fsm.push_event(success_e{});
}

template<class Table>
void HIDTableSettings<Table>::init_characteristics_a::operator()(void) const
{
    fsm.clear_events();
    characteristic_id = HID_FIRST_KEY;
    index = 0;
    errors_count = 0;
    timer.start();
}

template<class Table>
void HIDTableSettings<Table>::iterate_characteristics_a::operator()(void) const
{
    index++;
    if (index >= hid_table.characteristicLength(characteristic_id)) {
        characteristic_id++;
        index = 0;
        errors_count = 0;
    }

    timer.start();
}

template<class Table>
void HIDTableSettings<Table>::start_init_a::operator()(void) const
{
    fsm.clear_events();
}

template<class Table>
void HIDTableSettings<Table>::start_idle_a::operator()(void) const
{
    fsm.clear_events();
}

template<class Table>
void HIDTableSettings<Table>::count_error_a::operator()(void) const
{
    fsm.clear_events();
    errors_count++;
    timer.start();
}

template<class Table>
void HIDTableSettings<Table>::reset_usb_a::operator()(void) const
{
    // TODO: reset usb
    throw exceptions::UsbException();
}

#endif // HIDTABLESETTINGS_H
