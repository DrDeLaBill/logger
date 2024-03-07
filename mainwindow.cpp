#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ctime>
#include <iostream> // TODO

#include <QMessageBox>

#include "log.h"
#include "hal_defs.h"

#include "usbcontroller.h"
#include "devicesettings.h"


#define TIME_STRING_LEN      (25)
#define INFO_TIMEOUT_MS      (200)
#define SAVE_TIMEOUT_MS      (5000)


Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();
USBRequestType MainWindow::requestType = USB_REQUEST_NONE;
USBController MainWindow::usbcontroller;
QTimer* MainWindow::saveTimer;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    MainWindow::setLoading();
    requestType = USB_REQUEST_LOAD_SETTINGS;
    usbcontroller.loadSettings();
    disableAll();

    QTimer* infoTimer = new QTimer(this);
    QObject::connect(infoTimer, QTimer::timeout, this, MainWindow::onInfoTimeout);
    infoTimer->start(INFO_TIMEOUT_MS);

    saveTimer = new QTimer(this);
    QObject::connect(saveTimer, QTimer::timeout, this, MainWindow::onSaveTimeout);
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

void MainWindow::on_updateTimeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }

    DeviceInfo::time{}.set(static_cast<uint32_t>(std::time(nullptr) - TIMESTAMP2000_01_01_00_00_00));
    DeviceInfo::time::updated = true;

    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveInfo();
    requestType = USB_REQUEST_SAVE_INFO;
}

void MainWindow::on_updateBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }
    MainWindow::setLoading();
    usbcontroller.loadSettings();
    requestType = USB_REQUEST_LOAD_SETTINGS;
}

void MainWindow::on_upgradeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }
    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveSettings();
    requestType = USB_REQUEST_SAVE_SETTINGS;
}

void MainWindow::onInfoTimeout()
{
    if (saveTimer->isActive()) {
        return;
    }
    if (requestType == USB_REQUEST_NONE) {
        usbcontroller.loadInfo();
        requestType = USB_REQUEST_LOAD_INFO;
    }
}

void MainWindow::onSaveTimeout()
{
    saveTimer->stop();
}

void MainWindow::responseProccess(const USBRequestType type, const USBCStatus status)
{
    if (saveTimer->isActive()) {
        requestType = USB_REQUEST_NONE;
        return;
    }

    if (status != USBC_RES_DONE) {
        requestType = USB_REQUEST_NONE;
        return;
    }

    MainWindow::resetLoading();

    if (!DeviceSettings::check()) {
        MainWindow::setError(exceptions::UnknownDeviceException().message.c_str());
        requestType = USB_REQUEST_NONE;
        return;
    }

    switch (type) {
    case USB_REQUEST_LOAD_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_SAVE_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_LOAD_INFO:
        showSettings();
        break;
    case USB_REQUEST_SAVE_INFO:
        showSettings();
        break;
    case USB_REQUEST_LOAD_LOG:
        MainWindow::setError(exceptions::USBExceptionGroup().message);
        break;
    default:
        MainWindow::setError(exceptions::UsbUndefinedBehaviourException().what());
        break;
    }

    requestType = USB_REQUEST_NONE;
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
    std::time_t tick = (std::time_t)(TIMESTAMP2000_01_01_00_00_00 + static_cast<uint64_t>(DeviceInfo::time{}.get()));
    struct tm tm;
    char strTime[TIME_STRING_LEN] = {};
    tm = *(std::localtime(&tick));
    std::strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", &tm);
    ui->updateTimeBtn->blockSignals(true);
    ui->time->setText(strTime);
    ui->updateTimeBtn->blockSignals(false);

    if (requestType == USB_REQUEST_LOAD_INFO) {
        return;
    }

    enableAll();

    ui->device_label->setText("Logger");
    std::string version = "v0." + std::to_string(DeviceSettings::fw_id{}.get()) + "." + std::to_string(DeviceSettings::sw_id{}.get());
    ui->version_label->setText(version.c_str());

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
    // TODO: load screen
    disableAll();
}

void MainWindow::resetLoading()
{
    ui->statusbar->showMessage("Ready");
    enableAll();
}

