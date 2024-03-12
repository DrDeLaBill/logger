#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <cstdint>
#include <variant>
#include <type_traits>

#include <QThread>
#include <QObject>
#include <QString>

#include <libusb.h>

#include "utils.h"

#include "usbcstatus.h"
#include "HIDController.h"
#include "hidtableworker.h"

#include "deviceinfo.h"
#include "devicerecord.h"
#include "devicesettings.h"


enum USBRequestType
{
    USB_REQUEST_NONE = 0,
    USB_REQUEST_LOAD_SETTINGS,
    USB_REQUEST_SAVE_SETTINGS,
    USB_REQUEST_LOAD_INFO,
    USB_REQUEST_SAVE_INFO,
    USB_REQUEST_LOAD_LOG
};


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void proccess(const USBRequestType type);

signals:
    void resultReady(const USBCStatus status);

private:
    static constexpr char TAG[] = "USBW";

    using table_settings_t = HIDTable<
        HIDTuple<uint16_t, DeviceSettings::dv_type>,
        HIDTuple<uint8_t,  DeviceSettings::sw_id>,
        HIDTuple<uint8_t,  DeviceSettings::fw_id>,
        HIDTuple<uint32_t, DeviceSettings::cf_id>,
        HIDTuple<uint32_t, DeviceSettings::record_period>,
        HIDTuple<uint32_t, DeviceSettings::send_period>,
        HIDTuple<uint32_t, DeviceSettings::record_id>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_status,    __arr_len(DeviceSettings::settings_t::modbus1_status)>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_value_reg, __arr_len(DeviceSettings::settings_t::modbus1_value_reg)>,
        HIDTuple<uint16_t, DeviceSettings::modbus1_id_reg,    __arr_len(DeviceSettings::settings_t::modbus1_id_reg)>
    >;
    using settings_worker_t = HIDTableWorker<table_settings_t, HID_FIRST_KEY>;
    static settings_worker_t handlerSettings;

    using table_info_t = HIDTable<
        HIDTuple<uint32_t, DeviceInfo::time>
    >;
    using info_worker_t = HIDTableWorker<table_info_t, settings_worker_t::maxID() + 1>;
    static info_worker_t handlerInfo;
    //    TableLog handlerLog;

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USBc";

    USBWorker worker;

    static USBRequestType requestType;

public:
    USBController();
    ~USBController();

    void loadSettings();
    void saveSettings();
    void loadInfo();
    void saveInfo();
    void loadLog();


public slots:
    void handleResults(const USBCStatus status);

signals:
    void request(const USBRequestType type);
    void responseReady(const USBRequestType type, const USBCStatus status);

};


#endif // USBWATCHER_H
