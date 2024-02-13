#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include "usbcontroller.h"



Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();

fsm::FiniteStateMachine<MainWindow::fsm_table> MainWindow::fsm;
USBCStatus MainWindow::curStatus = USBC_RES_OK;
utl::Timer MainWindow::timer(TIMER_DELAY_MS);
USBController MainWindow::usbcontroller;
unsigned MainWindow::errorsCount = 0;
bool MainWindow::fsmWait = false;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    connect();
}

MainWindow::~MainWindow() { }


void MainWindow::proccess(const USBCStatus& status)
{
    curStatus = status;
    fsm.proccess();
}

void MainWindow::setError(const QString& str)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
}

void MainWindow::connect()
{
    fsm.push_event(connect_e{});

    usbcontroller.proccess(USB_SEARCH_HANDLER);

    fsmWait = false;
}

void MainWindow::update()
{
    fsm.push_event(update_e{});
    USBUpdateHandler::update();

    fsm.proccess();
    usbcontroller.proccess(USB_UPDATE_HANDLER);

    fsmWait = false;
}

void MainWindow::upgrade()
{
    fsm.push_event(upgrade_e{});
    // TODO: USBUpgradeHandler::upgrade();

    fsm.proccess();
    usbcontroller.proccess(USB_UPGRADE_HANDLER);

    fsmWait = false;
}

void MainWindow::_init_s::operator()()
{
    fsmWait = false;

    fsm.push_event(success_e{});
    usbcontroller.proccess();
}

void MainWindow::_connect_s::operator()()
{
    fsmWait = false;

    switch (curStatus) {
    case USBC_RES_OK:
        usbcontroller.proccess(USB_SEARCH_HANDLER);
        break;
    case USBC_RES_DONE:
        fsm.push_event(success_e{});
        usbcontroller.proccess(USB_UPDATE_HANDLER);
        break;
    default:
        fsm.push_event(error_e{});
        usbcontroller.proccess(USB_SEARCH_HANDLER);
        break;
    }
}

void MainWindow::_load_settings_s::operator()()
{
    fsmWait = false;

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        errorsCount++;
    }

    if (errorsCount > ERRORS_MAX) {
        fsm.push_event(error_e{});
    }

    switch (curStatus) {
    case USBC_RES_DONE:
        fsm.push_event(success_e{});
        usbcontroller.proccess();
        break;
    default:
        timer.start();
        usbcontroller.proccess(USB_UPDATE_HANDLER);
        break;
    }
}

void MainWindow::_save_settings_s::operator()()
{
    fsmWait = false;

    if (!timer.wait()) {
        fsm.push_event(timeout_e{});
        errorsCount++;
    }

    if (errorsCount > ERRORS_MAX) {
        fsm.push_event(error_e{});
    }

    switch (curStatus) {
    case USBC_RES_DONE:
        fsm.push_event(success_e{});
        break;
    default:
        timer.start();
        usbcontroller.proccess(USB_UPGRADE_HANDLER);
        break;
    }
}

void MainWindow::_idle_s::operator()()
{
    if (fsmWait) {
        return;
    }

    fsmWait = true;
    fsm.proccess();
}

void MainWindow::_error_s::operator()()
{
    if (fsmWait) {
        return;
    }

    fsmWait = true;
    fsm.proccess();

    setError(exceptions::DeviceGroup().message);
}

void MainWindow::init_ui_a::operator()()
{
    // TODO: init UI
    errorsCount = 0;
}

void MainWindow::reset_ui_a::operator ()()
{
    // TODO: reset UI
    errorsCount = 0;
}

void MainWindow::start_timer_a::operator ()()
{
    timer.start();
}

void MainWindow::show_settings_a::operator ()()
{
    QString str = std::to_string(*(DeviceSettings::record_period{}())).c_str();
    ui->record_period->setText(str);

    str = std::to_string(*(DeviceSettings::send_period{}())).c_str();
    ui->send_period->setText(str);
}

void MainWindow::on_updateBtn_clicked()
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "update button clicked");
#endif
    if (fsmWait) {
        update();
    }
}


void MainWindow::on_upgradeBtn_clicked()
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "upgrade button clicked");
#endif
    if (fsmWait) {
        upgrade();
    }
}

