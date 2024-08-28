#ifndef COMTABLEWORKER_H
#define COMTABLEWORKER_H


#include <vector>

#include "gtime.h"

#include "Timer.h"
#include "FiniteStateMachine.h"

#include "gprotocol.h"
#include "comservice.h"
#include "usbcstatus.h"


#define COM_DELAY_MS (100)


struct TableWorker
{
protected:
    static constexpr unsigned ERRORS_COUNT_MAX = 50;

    static uint32_t curr_key;
    static uint32_t counter;
    static unsigned errors_count;
    static USBCStatus result;
    static utl::Timer timer;
    static uint8_t index;

    // Events:
    FSM_CREATE_EVENT(ready_e,   0);
    FSM_CREATE_EVENT(success_e, 0);
    FSM_CREATE_EVENT(update_e,  1);
    FSM_CREATE_EVENT(upgrade_e, 1);
    FSM_CREATE_EVENT(end_e,     2);
    FSM_CREATE_EVENT(timeout_e, 3);
    FSM_CREATE_EVENT(error_e,   4);

    static uint32_t value_hash(gprotocol::type_t value);

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
    struct init_a                    { void operator()(void) const; };
    struct error_a                   { void operator()(void) const; };
    struct iterate_update_a          { void operator()(void) const; };
    struct init_upgrade_a            { void operator()(void) const; };
    struct iterate_upgrade_a         { void operator()(void) const; };
    struct end_update_a              { void operator()(void) const; };
    struct end_upgrade_a             { void operator()(void) const; };
    struct count_error_a             { void operator()(void) const; };

    using fsm_table = fsm::TransitionTable<
        fsm::Transition<init_s,    success_e, idle_s,    init_a,            fsm::Guard::NO_GUARD>,
        fsm::Transition<init_s,    update_e,  update_s,  init_a,            fsm::Guard::NO_GUARD>,
        fsm::Transition<init_s,    upgrade_e, upgrade_s, init_upgrade_a,    fsm::Guard::NO_GUARD>,

        fsm::Transition<idle_s,    update_e,  update_s,  init_a,            fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,    upgrade_e, upgrade_s, init_upgrade_a,    fsm::Guard::NO_GUARD>,

        fsm::Transition<update_s,  success_e, update_s,  iterate_update_a,  fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  timeout_e, update_s,  count_error_a,     fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  end_e,     idle_s,    end_update_a,      fsm::Guard::NO_GUARD>,
        fsm::Transition<update_s,  error_e,   init_s,    error_a,           fsm::Guard::NO_GUARD>,

        fsm::Transition<upgrade_s, success_e, upgrade_s, iterate_upgrade_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, timeout_e, upgrade_s, count_error_a,     fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, end_e,     init_s,    end_upgrade_a,     fsm::Guard::NO_GUARD>,
        fsm::Transition<upgrade_s, error_e,   init_s,    error_a,           fsm::Guard::NO_GUARD>
    >;

    static std::unordered_map<uint32_t, gtuple> table;
    static gprotocol protocol;
    static std::unordered_map<uint32_t, std::vector<uint32_t>> get_hashes;
    static std::unordered_map<uint32_t, std::vector<uint32_t>> set_hashes;

    USBCStatus usbSessionProccess() const;

protected:
    static fsm::FiniteStateMachine<fsm_table> fsm;

    static bool firstLoad;

public:
    static COMService* comService;

    USBCStatus load() const;
    USBCStatus save();
};

#endif // TableWorker_H
