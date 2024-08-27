#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <ctime>
#include <iostream> // TODO

#include <QtCore>
#include <QWidget>
#include <QMessageBox>
#include <QWheelEvent>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "app.h"
#include "glog.h" // TODO: remove
#include "settings.h"
#include "hal_defs.h"

#include "usbcontroller.h"
#include "app_exception.h"


#define TIME_STRING_LEN      (25)
#define INFO_TIMEOUT_MS      (200)
#define SAVE_TIMEOUT_MS      (10000)
#define SELECT_UPDATE_MS     (1000)


Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();
USBRequestType MainWindow::requestType = USB_REQUEST_NONE;
USBController MainWindow::usbcontroller;
QTimer* MainWindow::saveTimer;
QTimer* MainWindow::infoTimer;

SensorList* MainWindow::modbus1ListBox;
ModbusBox* MainWindow::firstmodbus1Sensor;
std::vector<ModbusBox> MainWindow::modbus1Sensors;
std::vector<QMetaObject::Connection> MainWindow::m_modbus1Connection;

SensorList* MainWindow::onewireListBox;
OneWireService* MainWindow::oneWireService;
std::vector<OneWireBox> MainWindow::oneWireSensors;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);

    QObject::connect(&usbcontroller, &usbcontroller.responseReady, this, responseProccess);
    QObject::connect(&usbcontroller, &usbcontroller.error, this, onUSBError);

    modbus1ListBox     = new SensorList(ui->groupBox_2, "MODBUS1");
    firstmodbus1Sensor = new ModbusBox(modbus1ListBox->sensors_group, {"+", 0, 0, 0, 0, 0});
    QObject::connect(firstmodbus1Sensor, &firstmodbus1Sensor->save, this, onSaveModbus1Sensor);
    firstmodbus1Sensor->show();

    onewireListBox = new SensorList(ui->groupBox_2, "ONEWIRE");
    oneWireService = new OneWireService(onewireListBox->sensors_group);
    QObject::connect(oneWireService, &oneWireService->registerClicked, this, onOneWireRegister);
    oneWireService->show();

    infoTimer = new QTimer(this);
    QObject::connect(infoTimer, QTimer::timeout, this, MainWindow::onInfoTimeout);

    saveTimer = new QTimer(this);
    QObject::connect(saveTimer, QTimer::timeout, this, MainWindow::onSaveTimeout);

    QObject::connect(
        modbus1ListBox->verticalScrollBar,
        &modbus1ListBox->verticalScrollBar->valueChanged,
        this,
        on_verticalScrollBar_valueChanged
    );
    QObject::connect(
        onewireListBox->verticalScrollBar,
        &onewireListBox->verticalScrollBar->valueChanged,
        this,
        on_verticalScrollBar_valueChanged
    );

    ui->updatePortsBtn->click();
    ui->modbus1Btn->click();

#ifndef DEBUG
    ui->progressBar->hide();
    ui->speed->hide();
#endif
}

MainWindow::~MainWindow()
{
    infoTimer->deleteLater();
    saveTimer->deleteLater();

    clearSensors();
    firstmodbus1Sensor->deleteLater();
    delete modbus1ListBox;

    oneWireService->deleteLater();
    delete onewireListBox;
}

void MainWindow::setError(const QString& str)
{
    ui->statusbar->showMessage(str);
    ui->device_label->setText("Logger error");
    QMessageBox messageBox;
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

void MainWindow::on_updateBtn_clicked()
{
    clearSensors();
    usbcontroller.loadSettings(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_LOAD_SETTINGS;
}

void MainWindow::on_upgradeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }

    app_info.time = static_cast<uint32_t>(std::time(nullptr) - TIMESTAMP2000_01_01_00_00_00);

    settings.record_period = ui->record_period->toPlainText().toUInt();
    settings.send_period = ui->send_period->toPlainText().toUInt();

    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveSettings(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_SAVE_SETTINGS;
}

void MainWindow::onInfoTimeout()
{
    if (saveTimer->isActive()) {
        return;
    }

    if (requestType == USB_REQUEST_NONE) {
        usbcontroller.loadSettings(ui->serialPortSelect->currentText());
        requestType = USB_REQUEST_LOAD_SETTINGS;
    }

    if (app_info.need_registrate_1wire) {
        showOneWireSensors();
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

    if (!settings_check(&settings)) {
#if defined(QT_NO_DEBUG)
        MainWindow::setError(exceptions::USBExceptionGroup().message);
#else
        MainWindow::setError(exceptions::UnknownDeviceException().message.c_str());
#endif
        return;
    }

    switch (type) {
    case USB_REQUEST_LOAD_SETTINGS:
        showSettings(type);
        saveTimer->stop();
        break;
    case USB_REQUEST_SAVE_SETTINGS:
        ui->updateBtn->setDisabled(false);
        ui->updateBtn->click();
        break;
    default:
#if defined(QT_NO_DEBUG)
        MainWindow::setError(exceptions::InternallExceptionGroup().message);
#else
        MainWindow::setError(exceptions::UsbUndefinedBehaviourException().what());
#endif
        break;
    }

    if (!infoTimer->isActive()) {
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

    infoTimer->stop();

    if (firstmodbus1Sensor) {
        firstmodbus1Sensor->disable();
    }
    for (auto& sensor : modbus1Sensors) {
        sensor.disable();
    }
    for (auto& sensor : oneWireSensors) {
        sensor.disable();
    }

    oneWireService->disable();
}

void MainWindow::enableAll()
{
    ui->time->setDisabled(false);
    ui->record_period->setDisabled(false);
    ui->send_period->setDisabled(false);

    ui->updateBtn->setDisabled(false);
    ui->upgradeBtn->setDisabled(false);

    if (firstmodbus1Sensor) {
        firstmodbus1Sensor->enable();
    }
    for (auto& sensor : modbus1Sensors) {
        sensor.enable();
    }
    for (auto& sensor : oneWireSensors) {
        sensor.enable();
    }

    oneWireService->enable();
}

void MainWindow::updateScrollBar()
{
    int value = modbus1ListBox->verticalScrollBar->value();
    modbus1ListBox->verticalScrollBar->blockSignals(true);
    modbus1ListBox->verticalScrollBar->setMinimum(0);
    modbus1ListBox->verticalScrollBar->setMaximum(
        (modbus1Sensors.size() + 1) * MODBUS_BOX_HEIGHT - modbus1ListBox->sensors_group->geometry().height()
    );
    modbus1ListBox->verticalScrollBar->setValue(value);
    modbus1ListBox->verticalScrollBar->blockSignals(false);


    value = onewireListBox->verticalScrollBar->value();
    onewireListBox->verticalScrollBar->blockSignals(true);
    onewireListBox->verticalScrollBar->setMinimum(0);
    onewireListBox->verticalScrollBar->setMaximum(
        (oneWireSensors.size() + 1) * ONEWIRE_BOX_HEIGHT - onewireListBox->sensors_group->geometry().height()
    );
    onewireListBox->verticalScrollBar->setValue(value);
    onewireListBox->verticalScrollBar->blockSignals(false);
}

void MainWindow::updateCOMSelect()
{
    ui->serialPortSelect->clear();
    ui->serialPortSelect->addItem("none");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->serialPortSelect->addItem(info.portName());
    }
}

void MainWindow::setMODBUS1Hidden(bool state)
{
    QString style(
        "QPushButton {"
        "border-bottom: 0px;"
        "border-top-left-radius: 0;"
        "border-top-right-radius: 2px;"
        "border-bottom-left-radius: 0;"
        "border-bottom-right-radius: 0;"
        "background-color: "
    );
    style += state ? "#515a5a; }" : "#424949; }";
    ui->modbus1Btn->setStyleSheet(style);

    if (state) {
        ui->modbus1Header->hide();
        modbus1ListBox->sensors_group->hide();
        modbus1ListBox->verticalScrollBar->valueChanged(0);
    } else {
        ui->modbus1Header->show();
        modbus1ListBox->sensors_group->show();
    }
}

void MainWindow::setONEWIREHidden(bool state)
{
    QString style(
        "QPushButton {"
            "border-bottom: 0px;"
            "border-top-left-radius: 0;"
            "border-top-right-radius: 2px;"
            "border-bottom-left-radius: 0;"
            "border-bottom-right-radius: 0;"
            "background-color: "
    );
    style += state ? "#515a5a; }" : "#424949; }";
    ui->onewireBtn->setStyleSheet(style);

    if (state) {
        ui->onewireHeader->hide();
        onewireListBox->sensors_group->hide();
        onewireListBox->verticalScrollBar->valueChanged(0);
    } else {
        ui->onewireHeader->show();
        onewireListBox->sensors_group->show();
    }
}

void MainWindow::clearSensors()
{
    for (auto connection : m_modbus1Connection) {
        QObject::disconnect(connection);
    }
    m_modbus1Connection.clear();
    modbus1Sensors.clear();
    oneWireSensors.clear();
}

void MainWindow::showSettings(const USBRequestType type)
{
    static settings_t hashSettings = {};

    std::time_t tick = (std::time_t)(TIMESTAMP2000_01_01_00_00_00 + static_cast<uint64_t>(app_info.time));
    struct tm tm;
    char strTime[TIME_STRING_LEN] = {};
    tm = *(std::localtime(&tick));
    std::strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", &tm);
    if (ui->time->text() != QString(strTime)) {
        ui->time->setText(strTime);
    }

    for (auto& sensor : modbus1Sensors) {
        int16_t value = app_info.modbus1_last_value[sensor.getID() - 1];
        if (value == std::numeric_limits<int16_t>::max()) {
            sensor.setValue("ERR");
        } else {
            sensor.setValue(std::to_string(value).c_str());
        }
    }

    for (unsigned i = 0; i < oneWireSensors.size(); i++) {
        int16_t value = app_info._1wire_last_value[i];
        if (value == std::numeric_limits<int16_t>::max()) {
            oneWireSensors.at(i).setValue("ERR");
        } else {
            char value_str[20] = "";
            snprintf(value_str, sizeof(value_str), "%d.%d", value / 10, __abs(value % 10));
            oneWireSensors.at(i).setValue(value_str);
        }
    }

    if (oneWireService->isRegistratinig() != app_info.need_registrate_1wire) {
        app_info.need_registrate_1wire ?
            oneWireService->start() :
            oneWireService->stop();
    }

    if (type != USB_REQUEST_LOAD_SETTINGS) {
        return;
    }

    if (util_hash((uint8_t*)&hashSettings, sizeof(hashSettings)) ==
        util_hash((uint8_t*)&settings, sizeof(hashSettings))
    ) {
        return;
    }

    memcpy((uint8_t*)&hashSettings, (uint8_t*)&settings, sizeof(settings));

    enableAll();

    ui->device_label->setText(std::string(
        std::string("Logger v") +
        std::string("0.") + // TODO: add version parameter
        std::to_string(settings.fw_id) +
        std::string(".") +
        std::to_string(settings.sw_id)
    ).c_str());

    ui->record_period->blockSignals(true);
    ui->record_period->setText(std::to_string(settings.record_period).c_str());
    ui->record_period->blockSignals(false);

    ui->send_period->blockSignals(true);
    ui->send_period->setText(std::to_string(settings.send_period).c_str());
    ui->send_period->blockSignals(false);

    firstmodbus1Sensor->clear();

    clearSensors();
    for (unsigned  i = 0; i < __arr_len(settings.modbus1_status); i++) {
        i = modbus1_index(i);

        if (i >= __arr_len(settings.modbus1_status)) {
            break;
        }

        int16_t value = app_info.modbus1_last_value[i];

        modbus1Sensors.push_back({
            modbus1ListBox->sensors_group,
            {
                "U",
                i + 1,
                static_cast<int>(modbus1Sensors.size()) + 1, // TODO: warn
                settings.modbus1_id_reg[i],
                settings.modbus1_value_reg[i],
                value
            }
        });
    }

    for (auto& sensor : modbus1Sensors) {
        m_modbus1Connection.push_back(
            QObject::connect(&sensor, &sensor.save, this, onSaveModbus1Sensor)
        );
    }

    if (!modbus1ListBox->sensors_group->isHidden()) {
        modbus1ListBox->sensors_group->show();
        for (unsigned i = 0; i < modbus1Sensors.size(); i++) {
            modbus1Sensors.at(i).show();
        }
    }

    showOneWireSensors();

    updateScrollBar();
}

void MainWindow::showOneWireSensors()
{
    oneWireSensors.clear();

    int offset = oneWireService->getY();
    for (unsigned  i = 0; i < __arr_len(settings._1wire_address); i++) {
        i = _1wire_index(i);

        if (i >= __arr_len(settings._1wire_address)) {
            break;
        }

        OneWireData data{};
        data.number  = static_cast<int>(oneWireSensors.size()) + 1;
        data.address = settings._1wire_address[i];
        data.value   = app_info._1wire_last_value[i];
        oneWireSensors.push_back({
            onewireListBox->sensors_group,
            data,
            offset
        });
    }

    if (!onewireListBox->sensors_group->isHidden()) {
        onewireListBox->sensors_group->show();
        for (unsigned i = 0; i < oneWireSensors.size(); i++) {
            oneWireSensors.at(i).show();
        }
    }

    updateScrollBar();
}

void MainWindow::setLoading()
{
    ui->statusbar->showMessage("Loading...");
    infoTimer->stop();
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
    if (!modbus1ListBox->sensors_group->isHidden()) {
        int delta = 0;
        int result = firstmodbus1Sensor->getY() - value;
        if (value < 0) {
            delta = -result;
            result = 0;
        }
        firstmodbus1Sensor->setY(result);
        for (unsigned i = 0; i < modbus1Sensors.size(); i++) {
            ModbusBox& tmp = modbus1Sensors.at(i);
            tmp.setY(tmp.getY() - value + delta);
        }
    }
    if (!onewireListBox->sensors_group->isHidden()) {
        int delta = 0;
        int result = -value;
        if (value < 0) {
            delta = -result;
            result = 0;
        }
        oneWireService->setY(result);
        for (unsigned i = 0; i < oneWireSensors.size(); i++) {
            OneWireBox& tmp = oneWireSensors.at(i);
            tmp.setY(tmp.calculateY() - value + delta);
        }
    }
}

void MainWindow::onSaveModbus1Sensor(const ModbusData& sensorData)
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
        settings.modbus1_status[index] != SETTINGS_SENSOR_EMPTY
    ) {
        MainWindow::setWarning("The sensor ID is already busy");
        return;
    }

    if (sensorData.lastID > 0 && sensorData.lastID != sensorData.sensorID) {
        app_info.mb1_last_id = sensorData.lastID;
        app_info.mb1_new_id = sensorData.sensorID;
        app_info.need_mb1_id_update = 1;


        settings.modbus1_status[index] = SETTINGS_SENSOR_EMPTY;
        settings.modbus1_id_reg[index] = 0;
        settings.modbus1_value_reg[index] = 0;

        index = sensorData.sensorID;
        if (index == 0) {
            ui->upgradeBtn->click();
            return;
        }
        index--;
    }

    settings.modbus1_status[index] = SETTINGS_SENSOR_THERMAL; // TODO: select
    settings.modbus1_id_reg[index] = sensorData.idReg;
    settings.modbus1_value_reg[index] = sensorData.valueReg;

    ui->upgradeBtn->click();
}

void MainWindow::onOneWireRegister()
{
    if (oneWireService->isRegistratinig()) {
        oneWireService->stop();
        app_info.need_registrate_1wire = 0;

        saveTimer->stop();
        ui->updateBtn->click();
    } else {
        oneWireService->start();
        app_info.need_registrate_1wire = 1;

        saveTimer->start(SAVE_TIMEOUT_MS);
    }
    usbcontroller.saveSettings(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_SAVE_SETTINGS;
}

void QWidget::wheelEvent(QWheelEvent *event)
{
    if (MainWindow::modbus1ListBox->isCursorInside() &&
        !MainWindow::modbus1ListBox->sensors_group->isHidden()
    ) {
        MainWindow::scrollModbus1(event->angleDelta().y());
    }
    if (MainWindow::onewireListBox->isCursorInside() &&
        !MainWindow::onewireListBox->sensors_group->isHidden()
    ) {
        MainWindow::scrollOneWire(event->angleDelta().y());
    }
}

void MainWindow::scrollModbus1(int value)
{
    if (MainWindow::modbus1Sensors.empty()) {
        return;
    }
    int y = MainWindow::modbus1Sensors.back().getY() + MainWindow::modbus1Sensors.back().height();
    if (!MainWindow::modbus1ListBox->sensors_group->isHidden() &&
        y < MainWindow::modbus1ListBox->sensors_group->height()
        ) {
        return;
    }
    value > 0 ?
    MainWindow::modbus1ListBox->mouseWheelUp() :
    MainWindow::modbus1ListBox->mouseWheelDown();
}

void MainWindow::scrollOneWire(int value)
{
    if (MainWindow::oneWireSensors.empty()) {
        return;
    }
    int y = MainWindow::oneWireSensors.back().calculateY() + MainWindow::oneWireSensors.back().height();
    if (!MainWindow::onewireListBox->sensors_group->isHidden() &&
        y < MainWindow::onewireListBox->sensors_group->height()
        ) {
        return;
    }
    value > 0 ?
    MainWindow::onewireListBox->mouseWheelUp() :
    MainWindow::onewireListBox->mouseWheelDown();
}

void MainWindow::on_serialPortSelect_activated(int)
{
    ui->updateBtn->setDisabled(false);
    ui->updateBtn->click();
    ui->updateBtn->setDisabled(true);
}

void MainWindow::on_updatePortsBtn_clicked()
{
    updateCOMSelect();
    disableAll();
}


void MainWindow::on_modbus1Btn_clicked()
{
    setONEWIREHidden(true);
    setMODBUS1Hidden(false);
}


void MainWindow::on_onewireBtn_clicked()
{
    setMODBUS1Hidden(true);
    setONEWIREHidden(false);
}

