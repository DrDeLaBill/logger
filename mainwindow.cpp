#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ctime>
#include <iostream> // TODO

#include <QMessageBox>

#include "log.h"
#include "hal_defs.h"

#include "usbcontroller.h"


#define TIME_STRING_LEN (25)


Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();
USBController MainWindow::usbcontroller;
bool MainWindow::waitResponse = false;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    MainWindow::setLoading();
    usbcontroller.loadSettings();
    disableAll();
}

MainWindow::~MainWindow() { }

void MainWindow::setError(const QString& str)
{
    ui->statusbar->showMessage(str);
    ui->device_label->setText("Logger");
    ui->version_label->setText("error");
    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
    disableAll();
}

void MainWindow::on_updateBtn_clicked()
{
    if (!waitResponse) {
        MainWindow::setLoading();
        usbcontroller.loadSettings();
        waitResponse = true;
    }
}

void MainWindow::on_upgradeBtn_clicked()
{
    if (!waitResponse) {
        MainWindow::setLoading();
        usbcontroller.saveSettings();
        waitResponse = true;
    }
}

void MainWindow::responseProccess(const USBRequestType type, const USBCStatus status)
{
    waitResponse = false;

    if (status != USBC_RES_DONE) {
        return;
    }

    MainWindow::resetLoading();

    if (!DeviceSettings::check()) {
        MainWindow::setError(exceptions::UnknownDeviceException().message.c_str());
        return;
    }

    switch (type) {
    case USB_REQUEST_LOAD_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_SAVE_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_LOAD_LOG:
        MainWindow::setError(exceptions::USBExceptionGroup().message);
        break;
    default:
        throw exceptions::UsbUndefinedBehaviourException();
    }
}

void MainWindow::disableAll()
{
    ui->time->setDisabled(true);
    ui->record_period->setDisabled(true);
    ui->send_period->setDisabled(true);

    ui->upgradeBtn->setDisabled(true);
    ui->updateTimeBtn->setDisabled(true);
}

void MainWindow::enableAll()
{
    ui->time->setDisabled(false);
    ui->record_period->setDisabled(false);
    ui->send_period->setDisabled(false);

    ui->upgradeBtn->setDisabled(false);
    ui->updateTimeBtn->setDisabled(false);
}

void MainWindow::showSettings()
{
    enableAll();

    ui->device_label->setText("Logger");
    std::string version = "v0." + std::to_string(DeviceSettings::fw_id{}.get()) + "." + std::to_string(DeviceSettings::sw_id{}.get());
    ui->version_label->setText(version.c_str());

    std::time_t tick = (std::time_t)(TIMESTAMP2000_01_01_00_00_00 + static_cast<uint64_t>(DeviceSettings::time{}.get()));
    struct tm tm;
    char strTime[TIME_STRING_LEN] = {};
    tm = *(std::localtime(&tick));
    std::strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", &tm);
    ui->updateTimeBtn->blockSignals(true);
    ui->time->setText(strTime);
    ui->updateTimeBtn->blockSignals(false);

    ui->record_period->blockSignals(true);
    ui->record_period->setText(std::to_string(DeviceSettings::record_period{}.get()).c_str());
    ui->record_period->blockSignals(false);

    ui->send_period->blockSignals(true);
    ui->send_period->setText(std::to_string(DeviceSettings::send_period{}.get()).c_str());
    ui->send_period->blockSignals(false);
}

void MainWindow::on_record_period_textChanged()
{
    DeviceSettings::record_period{}.set(ui->record_period->toPlainText().toUInt());
    DeviceSettings::record_period::updated = true;
}

void MainWindow::on_send_period_textChanged()
{
    DeviceSettings::send_period{}.set(ui->send_period->toPlainText().toUInt());
    DeviceSettings::send_period::updated = true;
}

void MainWindow::setLoading()
{
    ui->statusbar->showMessage("Loading...");
}

void MainWindow::resetLoading()
{
    ui->statusbar->showMessage("Ready");
}

void MainWindow::on_updateTimeBtn_clicked()
{
    DeviceSettings::time{}.set(static_cast<uint32_t>(std::time(nullptr) - TIMESTAMP2000_01_01_00_00_00));
    DeviceSettings::time::updated = true;
    ui->upgradeBtn->click();
}

