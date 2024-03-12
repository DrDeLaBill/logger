#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ctime>
#include <iostream> // TODO

#include <QtCore>
#include <QWidget>
#include <QMessageBox>
#include <QWheelEvent>

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
bool MainWindow::needReconnectSlots = false;
SensorList* MainWindow::sensorListBox;
SensorBox* MainWindow::firstSensor;
std::vector<SensorBox> MainWindow::sensors;
std::vector<QMetaObject::Connection> MainWindow::m_sensorConnection;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);

    QObject::connect(&usbcontroller, &usbcontroller.responseReady, this, responseProccess);

    disableAll();
    MainWindow::setLoading();

    sensorListBox = new SensorList(ui->groupBox_2);
    firstSensor   = new SensorBox(sensorListBox->sensors_group, {"add", 0, 0, 0, 0, 0});
    QObject::connect(firstSensor, &firstSensor->save, this, onSaveSensor);
    firstSensor->show();

    QTimer* infoTimer = new QTimer(this);
    QObject::connect(infoTimer, QTimer::timeout, this, MainWindow::onInfoTimeout);
    infoTimer->start(INFO_TIMEOUT_MS);

    saveTimer = new QTimer(this);
    QObject::connect(saveTimer, QTimer::timeout, this, MainWindow::onSaveTimeout);

    requestType = USB_REQUEST_LOAD_SETTINGS;
    ui->updateBtn->click();
    ui->updateBtn->click();

    updateScrollBar();
    QMetaObject::connectSlotsByName(this);
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
    if (needReconnectSlots) {
        QMetaObject::connectSlotsByName(this);
        needReconnectSlots = false;
    }
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

void MainWindow::updateScrollBar()
{
    sensorListBox->verticalScrollBar->blockSignals(true);
    sensorListBox->verticalScrollBar->setValue(0);
    sensorListBox->verticalScrollBar->setMinimum(0);
    sensorListBox->verticalScrollBar->setMaximum(
        (sensors.size() + 1) * SENSOR_BOX_HEIGHT - sensorListBox->sensors_group->geometry().height()
    );
    sensorListBox->verticalScrollBar->blockSignals(false);

    needReconnectSlots = true;
}

void MainWindow::clearSensors()
{
    if (sensors.size() == 0) {
        return;
    }
    for (auto connection : m_sensorConnection) {
        QObject::disconnect(connection);
    }
    m_sensorConnection.clear();
    sensors.clear();
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

    clearSensors();
    unsigned index = 0;
    while (index < __arr_len(DeviceSettings::settings_t::modbus1_status)) {
        index = DeviceSettings::getIndex(index);

        if (index == __arr_len(DeviceSettings::settings_t::modbus1_status)) {
            break;
        }

        sensors.push_back({
            sensorListBox->sensors_group,
            {
                "save",
                index + 1,
                sensors.size() + 1,
                DeviceSettings::modbus1_id_reg{}.get(index),
                DeviceSettings::modbus1_value_reg{}.get(index),
                0
            }
        });

        index++;
    }

    for (auto& sensor : sensors) {
        m_sensorConnection.push_back(
            QObject::connect(&sensor, &sensor.save, this, onSaveSensor)
        );
    }

    sensorListBox->sensors_group->show();
    for (unsigned i = 0; i < sensors.size(); i++) {
        sensors.at(i).show();
    }

    updateScrollBar();
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


void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
    firstSensor->setY(firstSensor->getY() - value);
    for (unsigned i = 0; i < sensors.size(); i++) {
        SensorBox& tmp = sensors.at(i);
        tmp.setY(tmp.getY() - value);
    }
}

void MainWindow::onSaveSensor(const SensorData& sensorData)
{
    if (sensorData.lastID != sensorData.sensorID) {
        // TODO: update ID
        // TODO: emit update ID
    }

    DeviceSettings::modbus1_id_reg{}.set(sensorData.idReg, sensorData.lastID);
    DeviceSettings::modbus1_id_reg::updated = true;

    DeviceSettings::modbus1_value_reg{}.set(sensorData.valueReg, sensorData.lastID);
    DeviceSettings::modbus1_value_reg::updated = true;

    // TODO: emit update regs
    ui->upgradeBtn->click();
}

void QWidget::wheelEvent(QWheelEvent *event)
{
    if (!MainWindow::sensorListBox->isCursorInside()) {
        return;
    }
    if (MainWindow::sensors.empty()) {
        return;
    }
    if (MainWindow::sensors.back().getY() < MainWindow::sensorListBox->sensors_group->height()) {
        return;
    }
    if (event->angleDelta().y() > 0) {
        MainWindow::sensorListBox->mouseWheelUp();
    } else {
        MainWindow::sensorListBox->mouseWheelDown();
    }
}

