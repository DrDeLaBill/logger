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

#include "log.h" // TODO: remove
#include "hal_defs.h"

#include "usbcontroller.h"
#include "devicesettings.h"


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
    QObject::connect(&usbcontroller, &usbcontroller.loadLogProgressUpdated, this, onLoadLogProgressUpdated);

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

void MainWindow::on_updateTimeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }

    DeviceInfo::time{}.set(static_cast<uint32_t>(std::time(nullptr) - TIMESTAMP2000_01_01_00_00_00));
    DeviceInfo::time::updated[0] = true;

    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveInfo(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_SAVE_INFO;
}

void MainWindow::on_updateBtn_clicked()
{
    clearSensors();
    MainWindow::setLoading();
    usbcontroller.loadSettings(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_LOAD_SETTINGS;
}

void MainWindow::on_upgradeBtn_clicked()
{
    if (saveTimer->isActive()) {
        return;
    }

    saveTimer->start(SAVE_TIMEOUT_MS);
    MainWindow::setLoading();
    usbcontroller.saveSettings(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_SAVE_SETTINGS;
}

void MainWindow::on_dumpBtn_clicked()
{
    MainWindow::setLoading();
    usbcontroller.loadLog(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_LOAD_LOG;
}

void MainWindow::onLoadLogProgressUpdated(uint32_t value)
{
    static unsigned long long speedStart = 0;
    static uint32_t startId = 0;
    if (DeviceInfo::max_id::get() - DeviceInfo::min_id::get() == 0) {
        return;
    }
    unsigned percent = 100 * value / (DeviceInfo::max_id::get() - DeviceInfo::min_id::get());
    if (ui->progressBar->text().toStdString() == "0%") {
        speedStart = getMillis();
        startId = DeviceInfo::current_id::get();
    }
    unsigned long long factor = (unsigned long long)(value - startId) * 60000;
    unsigned long long time = (getMillis() - speedStart) + 1;
    if (ui->progressBar->text().toStdString() == "100%") {
        ui->speed->setText((std::to_string(factor / time) + " records/min " + std::to_string(time / 1000) + " sec").c_str());
    } else {
        ui->speed->setText((std::to_string(factor / time) + " records/min").c_str());
    }
    ui->progressBar->setValue(percent);
}

void MainWindow::onInfoTimeout()
{
    if (saveTimer->isActive()) {
        return;
    }

    if (requestType == USB_REQUEST_NONE) {
        usbcontroller.loadInfo(ui->serialPortSelect->currentText());
        requestType = USB_REQUEST_LOAD_INFO;
    }

    if (DeviceInfo::need_registrate_1wire::get()) {
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
    case USB_REQUEST_LOAD_LOG:
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

    ui->dumpBtn->setDisabled(true);
    ui->updateBtn->setDisabled(true);
    ui->upgradeBtn->setDisabled(true);
    ui->updateTimeBtn->setDisabled(true);

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

    ui->dumpBtn->setDisabled(false);
    ui->updateBtn->setDisabled(false);
    ui->upgradeBtn->setDisabled(false);
    ui->updateTimeBtn->setDisabled(false);

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
    std::time_t tick = (std::time_t)(TIMESTAMP2000_01_01_00_00_00 + static_cast<uint64_t>(DeviceInfo::time{}.get()));
    struct tm tm;
    char strTime[TIME_STRING_LEN] = {};
    tm = *(std::localtime(&tick));
    std::strftime(strTime, sizeof(strTime), "%Y-%m-%d %H:%M:%S", &tm);
    ui->updateTimeBtn->blockSignals(true);
    ui->time->setText(strTime);
    ui->updateTimeBtn->blockSignals(false);

    for (auto& sensor : modbus1Sensors) {
        uint16_t value = DeviceInfo::modbus1_last_value::get(sensor.getID() - 1);
        if (value == std::numeric_limits<uint16_t>::max()) {
            sensor.setValue("ERR");
        } else {
            sensor.setValue(std::to_string(value).c_str());
        }
    }

    for (auto& sensor : oneWireSensors) {
        uint16_t value = DeviceInfo::_1wire_last_value::get(sensor.getNumber());
        if (value == std::numeric_limits<uint16_t>::max()) {
            sensor.setValue("ERR");
        } else {
            sensor.setValue(std::to_string(value).c_str());
        }
    }

    if (oneWireService->isRegistratinig() && !DeviceInfo::need_registrate_1wire::get()) {
        oneWireService->stop();
    }

    if (type != USB_REQUEST_LOAD_SETTINGS) {
        return;
    }

    enableAll();

    ui->device_label->setText(std::string(
        std::string("Logger v") +
        std::string("0.") + // TODO: add version parameter
        std::to_string(DeviceSettings::fw_id{}.get()) +
        std::string(".") +
        std::to_string(DeviceSettings::sw_id{}.get())
    ).c_str());

    ui->record_period->blockSignals(true);
    ui->record_period->setText(std::to_string(DeviceSettings::record_period{}.get()).c_str());
    ui->record_period->blockSignals(false);

    ui->send_period->blockSignals(true);
    ui->send_period->setText(std::to_string(DeviceSettings::send_period{}.get()).c_str());
    ui->send_period->blockSignals(false);

    firstmodbus1Sensor->clear();

    clearSensors();
    for (unsigned  i = 0; i < __arr_len(DeviceSettings::settings_t::modbus1_status); i++) {
        i = DeviceSettings::getModbus1Index(i);

        if (i >= __arr_len(DeviceSettings::settings_t::modbus1_status)) {
            break;
        }

        uint16_t value = DeviceInfo::modbus1_last_value::get(i);

        modbus1Sensors.push_back({
            modbus1ListBox->sensors_group,
            {
                "U",
                i + 1,
                static_cast<int>(modbus1Sensors.size()) + 1,
                DeviceSettings::modbus1_id_reg::get(i),
                DeviceSettings::modbus1_value_reg::get(i),
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
    int offset = 0;
    if (!oneWireSensors.empty()) {
        oneWireSensors.back().getY();
    }

    oneWireSensors.clear();

    for (unsigned  i = 0; i < __arr_len(DeviceSettings::settings_t::_1wire_address); i++) {
        i = DeviceSettings::getOnewWireIndex(i);

        if (i >= __arr_len(DeviceSettings::settings_t::_1wire_address)) {
            break;
        }

        OneWireData data{};
        data.number  = static_cast<int>(oneWireSensors.size()) + 1;
        data.address = DeviceSettings::_1wire_address::get(i);
        data.value   = DeviceInfo::_1wire_last_value::get(i);
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
            tmp.setY(tmp.getY() - value + delta);
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

void MainWindow::onOneWireRegister()
{
    if (oneWireService->isRegistratinig()) {
        oneWireService->stop();
        DeviceInfo::need_registrate_1wire::set(0);
        DeviceInfo::need_registrate_1wire::updated[0] = true;

        saveTimer->stop();
        ui->updateBtn->click();
    } else {
        oneWireService->start();
        DeviceInfo::need_registrate_1wire::set(1);
        DeviceInfo::need_registrate_1wire::updated[0] = true;

        saveTimer->start(SAVE_TIMEOUT_MS);
    }
    usbcontroller.saveInfo(ui->serialPortSelect->currentText());
    requestType = USB_REQUEST_SAVE_INFO;
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
    int y = MainWindow::oneWireSensors.back().getY() + MainWindow::oneWireSensors.back().height();
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

