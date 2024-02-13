#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include <memory>

#include "Timer.h"
#include "FiniteStateMachine.h"

#include "usbcontroller.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void proccess(const USBCStatus& status);
    static void setError(const QString& str);

    static void connect();
    static void update();
    static void upgrade();

protected:
    static constexpr char TAG[] = "MAIN";

    static Ui::MainWindow* ui;

    // Events:
    FSM_CREATE_EVENT(success_e, 0);
    FSM_CREATE_EVENT(update_e,  0);
    FSM_CREATE_EVENT(upgrade_e, 0);
    FSM_CREATE_EVENT(connect_e, 0);
    FSM_CREATE_EVENT(timeout_e, 1);
    FSM_CREATE_EVENT(error_e,   2);

private:
    static constexpr unsigned TIMER_DELAY_MS = 1000;

    // States:
    struct _init_s          { void operator()(); };
    struct _connect_s       { void operator()(); };
    struct _load_settings_s { void operator()(); };
    struct _save_settings_s { void operator()(); };
    struct _idle_s          { void operator()(); };
    struct _error_s         { void operator()(); };

    FSM_CREATE_STATE(init_s,          _init_s);
    FSM_CREATE_STATE(connect_s,       _connect_s);
    FSM_CREATE_STATE(load_settings_s, _load_settings_s);
    FSM_CREATE_STATE(save_settings_s, _save_settings_s);
    FSM_CREATE_STATE(idle_s,          _idle_s);
    FSM_CREATE_STATE(error_s,         _error_s);

    // Actions:
    struct init_ui_a       { void operator()(); };
    struct reset_ui_a      { void operator()(); };
    struct start_timer_a   { void operator()(); };
    struct show_settings_a { void operator()(); };

    using fsm_table = fsm::TransitionTable<
        fsm::Transition<init_s,          success_e, connect_s,       init_ui_a,       fsm::Guard::NO_GUARD>,
        fsm::Transition<connect_s,       error_e,   connect_s,       reset_ui_a,      fsm::Guard::NO_GUARD>,
        fsm::Transition<connect_s,       success_e, load_settings_s, start_timer_a,   fsm::Guard::NO_GUARD>,
        fsm::Transition<load_settings_s, timeout_e, load_settings_s, start_timer_a,   fsm::Guard::NO_GUARD>,
        fsm::Transition<load_settings_s, error_e,   error_s,         reset_ui_a,      fsm::Guard::NO_GUARD>,
        fsm::Transition<load_settings_s, success_e, idle_s,          show_settings_a, fsm::Guard::NO_GUARD>,
        fsm::Transition<save_settings_s, timeout_e, save_settings_s, start_timer_a,   fsm::Guard::NO_GUARD>,
        fsm::Transition<save_settings_s, error_e,   error_s,         reset_ui_a,      fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,          update_e,  load_settings_s, start_timer_a,   fsm::Guard::NO_GUARD>,
        fsm::Transition<idle_s,          upgrade_e, save_settings_s, start_timer_a,   fsm::Guard::NO_GUARD>,
        fsm::Transition<error_s,         connect_e, connect_s,       init_ui_a,       fsm::Guard::NO_GUARD>
    >;

protected:
    static constexpr unsigned ERRORS_MAX = 5;

    static fsm::FiniteStateMachine<fsm_table> fsm;
    static USBController usbcontroller;
    static USBCStatus curStatus;
    static unsigned errorsCount;
    static utl::Timer timer;
    static bool fsmWait;

private slots:
    void on_updateBtn_clicked();
    void on_upgradeBtn_clicked();
};

#endif // MAINWINDOW_H
