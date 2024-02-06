#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <cstdint>
#include <variant>
#include <unordered_map>

#include <QThread>
#include <QObject>
#include <QString>

#include <libusb.h>
#include "usbhandler.h"
#include "Modules/HIDTable/HIDController.h"
#include "Modules/DeviceRecord/devicerecord.h"
#include "Modules/DeviceSettings/devicesettings.h"


#define USB_SEARCH_HANDLER             (QString("SEARCH_HANDLER"))
#define USB_REPORT_HANDLER             (QString("REPORT_HANDLER"))
#define USB_GET_CHARACTERISTIC_HANDLER (QString("GET_CHARACTERISITIC_HANDLER"))


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& parameter);
    void readCharacteristic(const uint16_t key, const uint8_t index = 0);

signals:
    void resultReady(const QString& result);
    void characteristicReady(const uint16_t key, const uint8_t* result, const uint8_t index = 0);

private:
    using handler_v = std::variant<
        USBSearchHandler,
        USBReportHandler,
        USBCGetharacteristicHandler
    >;
    using handler_t = std::unordered_map<
        QString,
        handler_v,
        USBHandlerHash,
        USBHandlerEqual
    >;

    handler_t handlers = {
        {USB_SEARCH_HANDLER, USBSearchHandler{}},
        {USB_REPORT_HANDLER, USBReportHandler{}},
        {USB_GET_CHARACTERISTIC_HANDLER, USBCGetharacteristicHandler{}}
    };

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USB";

    QString curParameter;

    using hid_table_t = HIDTable<
        HIDTuple<uint16_t, DeviceSettings::dv_type>,
        HIDTuple<uint8_t,  DeviceSettings::sw_id>,
        HIDTuple<uint8_t,  DeviceSettings::fw_id>,
        HIDTuple<uint32_t, DeviceSettings::cf_id>,
        HIDTuple<uint32_t, DeviceSettings::record_period>,
        HIDTuple<uint32_t, DeviceSettings::send_period>,
        HIDTuple<uint32_t, DeviceSettings::record_id>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_status>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_value_reg>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_id_reg>,
        HIDTuple<uint32_t, DeviceRecord::id>,
        HIDTuple<uint32_t, DeviceRecord::time>,
        HIDTuple<uint8_t,  DeviceRecord::IDs>,
        HIDTuple<uint16_t, DeviceRecord::values>
    >;

    HIDController<hid_table_t> hid_controller;

public:
    USBController();
    ~USBController();

    void proccess(const QString& parameter = USB_SEARCH_HANDLER);

public slots:
    void handleResults(const QString& parameter);
    void responseCharacteristic(const uint16_t key, const uint8_t* result, const uint8_t index = 0);

signals:
    void operate(const QString& parameter);
    void requestCharacteristic(const uint16_t key, const uint8_t index = 0);

};


#endif // USBWATCHER_H
