#ifndef USBHANDLER_H
#define USBHANDLER_H


#include <QString>

#include <cstdint>

#include "Timer.h"
#include "utils.h"
#include "HIDController.h"
#include "FiniteStateMachine.h"
#include "devicerecord.h"
#include "devicesettings.h"


struct USBHandlerEqual
{
    bool operator()(const QString& lhs, const QString& rhs) const;
};

struct USBHandlerHash
{
    std::size_t operator()(const QString& key) const;
};

struct USBSearchHandler
{
    void operate() const;
};

struct USBLoadRecordHandler // TODO: load
{
private:
protected:
    using hid_log_table_t = HIDTable<
        HIDTuple<uint32_t, DeviceRecord::id>,
        HIDTuple<uint32_t, DeviceRecord::time>,
        HIDTuple<uint8_t,  DeviceRecord::IDs>,
        HIDTuple<uint16_t, DeviceRecord::values>
    >;

    HIDController<hid_log_table_t> hid_log_controller;

public:
    void operate() const;
};

struct USBCGetCharacteristicHandler
{
protected:
    // Events:
    FSM_CREATE_EVENT(ready_e, 0);
    FSM_CREATE_EVENT(success_e, 0);
    FSM_CREATE_EVENT(end_e, 1);
    FSM_CREATE_EVENT(timeout_e, 2);
    FSM_CREATE_EVENT(error_e, 3);


private:
    // States:
    struct _init_s    {void operator()(void);};
    struct _idle_s    {void operator()(void); static utl::Timer timer;};
    struct _request_s {void operator()(void); static utl::Timer timer;};

    FSM_CREATE_STATE(init_s, _init_s);
    FSM_CREATE_STATE(idle_s, _idle_s);
    FSM_CREATE_STATE(request_s, _request_s);

    // Actions:
    struct init_characteristics_a    {void operator()(void);};
    struct iterate_characteristics_a {void operator()(void);};
    struct start_idle_timer_a        {void operator()(void);};
    struct count_error_a             {void operator()(void);};
    struct reset_usb_a               {void operator()(void);};

    using fsm_table = fsm::TransitionTable<
        fsm::Transition<init_s,    success_e, request_s, init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    timeout_e, request_s, init_characteristics_a,    fsm::Guard::NO_GUARD>,
        fsm::Transition<request_s, success_e, request_s, iterate_characteristics_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<request_s, timeout_e, request_s, count_error_a,             fsm::Guard::NO_GUARD>,
        fsm::Transition<request_s, end_e,     idle_s,    start_idle_timer_a,        fsm::Guard::NO_GUARD>,
        fsm::Transition<request_s, error_e,   init_s,    reset_usb_a,               fsm::Guard::NO_GUARD>
    >;

    void proccess();

protected:
    static constexpr unsigned ERRORS_COUNT_MAX = 5;

    static fsm::FiniteStateMachine<fsm_table> fsm;
    static uint16_t characteristic_id;
    static unsigned errors_count;
    static uint8_t index;

    using hid_settings_table_t = HIDTable<
        HIDTuple<uint16_t, DeviceSettings::dv_type>,
        HIDTuple<uint8_t,  DeviceSettings::sw_id>,
        HIDTuple<uint8_t,  DeviceSettings::fw_id>,
        HIDTuple<uint32_t, DeviceSettings::cf_id>,
        HIDTuple<uint32_t, DeviceSettings::record_period>,
        HIDTuple<uint32_t, DeviceSettings::send_period>,
        HIDTuple<uint32_t, DeviceSettings::record_id>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_status,    __arr_len(DeviceSettings::settings_t::modbus1_status)>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_value_reg, __arr_len(DeviceSettings::settings_t::modbus1_value_reg)>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_id_reg,    __arr_len(DeviceSettings::settings_t::modbus1_id_reg)>
    >;

    static HIDController<hid_settings_table_t> hid_settings_controller;

public:
    void operate() const;

};

struct USBCSetCharacteristicHandler
{
    void operate() const;
};


#endif // USBHANDLER_H
