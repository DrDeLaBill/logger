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
#define SAVE_TIMEOUT_MS      (10000)


Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();
USBRequestType MainWindow::requestType = USB_REQUEST_NONE;
USBController MainWindow::usbcontroller;
QTimer* MainWindow::saveTimer;
QTimer* MainWindow::infoTimer;
SensorList* MainWindow::sensorListBox;
SensorBox* MainWindow::firstSensor;
std::vector<SensorBox> MainWindow::sensors;
std::vector<QMetaObject::Connection> MainWindow::m_sensorConnection;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);

    QObject::connect(&usbcontroller, &usbcontroller.responseReady, this, responseProccess);
    QObject::connect(&usbcontroller, &usbcontroller.error, this, onUSBError);

    sensorListBox = new SensorList(ui->groupBox_2);
    firstSensor   = new SensorBox(sensorListBox->sensors_group, {"add", 0, 0, 0, 0, 0});
    QObject::connect(firstSensor, &firstSensor->save, this, onSaveSensor);
    firstSensor->show();

    infoTimer = new QTimer(this);
    QObject::connect(infoTimer, QTimer::timeout, this, MainWindow::onInfoTimeout);
    infoTimer->start(INFO_TIMEOUT_MS);

    saveTimer = new QTimer(this);
    QObject::connect(saveTimer, QTimer::timeout, this, MainWindow::onSaveTimeout);

    QObject::connect(
        sensorListBox->verticalScrollBar,
        &sensorListBox->verticalScrollBar->valueChanged,
        this,
        on_verticalScrollBar_valueChanged
    );

    ui->updateBtn->click();
}

MainWindow::~MainWindow()
{
    infoTimer->deleteLater();
    saveTimer->deleteLater();
    clearSensors();
    firstSensor->deleteLater();
    delete sensorListBox;
    QMainWindow::~QMainWindow();
}

void MainWindow::setError(const QString& str)
{
    ui->statusbar->showMessage(str);
    ui->device_label->setText("Logger");
    ui->version_label->setText("error");
    QMessageBox messageBox;
    if (infoTimer) {
        infoTimer->stop();
    }
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
    disableAll();
    ui->updateBtn->setDisabled(false);
}

void MainWindow::setWarning(const QString& str)
{
    ui->statusbar->showMessage(str);
    QMessageBox messageBox;
    messageBox.warning(0, "Warning", str);
    messageBox.setFixedSize(500, 200);
}

void MainWindow::on_updateTimeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }

    DeviceInfo::time{}.set(static_cast<uint32_t>(std::time(nullptr) - TIMESTAMP2000_01_01_00_00_00));
    DeviceInfo::time::updated[0] = true;

    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveInfo();
    requestType = USB_REQUEST_SAVE_INFO;

    if (infoTimer) {
        infoTimer->stop();
    }
}

void MainWindow::on_updateBtn_clicked()
{
    clearSensors();
    MainWindow::setLoading();
    usbcontroller.loadSettings();
    requestType = USB_REQUEST_LOAD_SETTINGS;

    if (infoTimer) {
        infoTimer->stop();
    }
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

    if (infoTimer) {
        infoTimer->stop();
    }
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

    if (infoTimer) {
        infoTimer->stop();
    }
}

void MainWindow::onSaveTimeout()
{
    saveTimer->stop();
#if defined(QT_NO_DEBUG)
    MainWindow::setError(exceptions::InternallExceptionGroup().message.toStdString().c_str());
#else
    MainWindow::setError(exceptions::SettingsSaveException().message.c_str());
#endif
}

void MainWindow::onUSBError(const QString &message)
{
    if (saveTimer->isActive()) {
        return;
    }

#if defined(QT_NO_DEBUG)
    MainWindow::setError(exceptions::USBExceptionGroup().message.toStdString().c_str());
#else
    MainWindow::setError(message.toStdString().c_str());
#endif
}

void MainWindow::responseProccess(const USBRequestType type, const USBCStatus status)
{
    if (requestType == type) {
        requestType = USB_REQUEST_NONE;
    } else {
        return;
    }

    // Wait load settings after save
    if (saveTimer->isActive() &&
        type   == USB_REQUEST_LOAD_SETTINGS &&
        status != USBC_RES_DONE
    ) {
        ui->updateBtn->setDisabled(false);
        ui->updateBtn->click();
        return;
    }

    if (status != USBC_RES_DONE) {
        return;
    }

    MainWindow::resetLoading();

    if (!DeviceSettings::check()) {
#if defined(QT_NO_DEBUG)
        MainWindow::setError(exceptions::USBExceptionGroup().message);
#else
        MainWindow::setError(exceptions::UnknownDeviceException().message.c_str());
#endif
        return;
    }

    switch (type) {
    case USB_REQUEST_LOAD_SETTINGS:
    case USB_REQUEST_LOAD_INFO:
        showSettings(type);
        saveTimer->stop();
        break;
    case USB_REQUEST_SAVE_SETTINGS:
    case USB_REQUEST_SAVE_INFO:
        ui->updateBtn->setDisabled(false);
        ui->updateBtn->click();
        break;
    case USB_REQUEST_LOAD_LOG: // TODO
    default:
#if defined(QT_NO_DEBUG)
        MainWindow::setError(exceptions::InternallExceptionGroup().message);
#else
        MainWindow::setError(exceptions::UsbUndefinedBehaviourException().what());
#endif
        break;
    }

    if (infoTimer) {
        infoTimer->start(INFO_TIMEOUT_MS);
    }
}

void MainWindow::disableAll()
{
    ui->time->setDisabled(true);
    ui->record_period->setDisabled(true);
    ui->send_period->setDisabled(true);

    ui->updateBtn->setDisabled(true);
    ui->upgradeBtn->setDisabled(true);
    ui->updateTimeBtn->setDisabled(true);

    if (firstSensor) {
        firstSensor->disable();
    }
    for (auto& sensor : sensors) {
        sensor.disable();
    }
}

void MainWindow::enableAll()
{
    ui->time->setDisabled(false);
    ui->record_period->setDisabled(false);
    ui->send_period->setDisabled(false);

    ui->updateBtn->setDisabled(false);
    ui->upgradeBtn->setDisabled(false);
    ui->updateTimeBtn->setDisabled(false);

    if (firstSensor) {
        firstSensor->enable();
    }
    for (auto& sensor : sensors) {
        sensor.enable();
    }
}

void MainWindow::updateScrollBar()
{
    sensorListBox->verticalScrollBar->blockSignals(true);
    sensorListBox->verticalScrollBar->setMinimum(0);
    sensorListBox->verticalScrollBar->setMaximum(
        (sensors.size() + 1) * SENSOR_BOX_HEIGHT - sensorListBox->sensors_group->geometry().height()
    );
    sensorListBox->verticalScrollBar->blockSignals(false);

    sensorListBox->verticalScrollBar->setValue(0);
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

void MainWindow::showSettings(const USBRequestType type)
{
    std::time_t tick = (std::time_t)(TIMESTAMP2000_01_01_00_00_00 + static_cast<uint64_t>(DeviceInfo::time{}.get()));
    struct tm tm;
    char strTime[TIME_STRING_LEN] = {};
    tm = *(std::localtime(&tick));
    std::strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", &tm);
    ui->updateTimeBtn->blockSignals(true);
    ui->time->setText(strTime);
    ui->updateTimeBtn->blockSignals(false);

    if (type != USB_REQUEST_LOAD_SETTINGS) {
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

    firstSensor->clear();

    clearSensors();
    unsigned index = 0;
    while (index < __arr_len(DeviceSettings::settings_t::modbus1_status)) {
        index = DeviceSettings::getIndex(index);

        if (index >= __arr_len(DeviceSettings::settings_t::modbus1_status)) {
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
    DeviceSettings::record_period::updated[0] = true;
}

void MainWindow::on_send_period_textChanged()
{
    DeviceSettings::send_period{}.set(ui->send_period->toPlainText().toUInt());
    DeviceSettings::send_period::updated[0] = true;
}

void MainWindow::setLoading()
{
    ui->statusbar->showMessage("Loading...");
    // TODO: loading screen
    disableAll();
}

void MainWindow::resetLoading()
{
    ui->statusbar->showMessage("Ready");
    enableAll();
}


void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
    int delta = 0;
    int result = firstSensor->getY() - value;
    if (value < 0) {
        delta = -result;
        result = 0;
    }
    firstSensor->setY(result);
    for (unsigned i = 0; i < sensors.size(); i++) {
        SensorBox& tmp = sensors.at(i);
        tmp.setY(tmp.getY() - value + delta);
    }
}

void MainWindow::onSaveSensor(const SensorData& sensorData)
{
    unsigned index = sensorData.lastID;
    if (sensorData.lastID == 0) {
        index = sensorData.sensorID;
    }
    if (sensorData.lastID == 0 && index == 0) { // TODO: remove sensorData.lastID == 0
        MainWindow::setWarning("Empty sensor ID");
        return;
    }
    index--;

    if (sensorData.lastID == 0 &&
        DeviceSettings::modbus1_status{}.get(index) != SETTINGS_SENSOR_EMPTY
    ) {
        MainWindow::setWarning("The sensor ID is already busy");
        return;
    }

    if (sensorData.lastID > 0 && sensorData.lastID != sensorData.sensorID) {
        DeviceSettings::modbus1_status{}.set(SETTINGS_SENSOR_EMPTY, index);
        DeviceSettings::modbus1_status::updated[index] = true;

        DeviceSettings::modbus1_id_reg{}.set(0, index);
        DeviceSettings::modbus1_id_reg::updated[index] = true;

        DeviceSettings::modbus1_value_reg{}.set(0, index);
        DeviceSettings::modbus1_value_reg::updated[index] = true;

        index = sensorData.sensorID;
        if (index == 0) {
            ui->upgradeBtn->click();
            return;
        }
        index--;
    }

    DeviceSettings::modbus1_status{}.set(SETTINGS_SENSOR_THERMAL, index); // TODO: select
    DeviceSettings::modbus1_status::updated[index] = true;

    DeviceSettings::modbus1_id_reg{}.set(sensorData.idReg, index);
    DeviceSettings::modbus1_id_reg::updated[index] = true;

    DeviceSettings::modbus1_value_reg{}.set(sensorData.valueReg, index);
    DeviceSettings::modbus1_value_reg::updated[index] = true;

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

