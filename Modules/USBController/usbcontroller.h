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

#include "devicesettings.h"
#include "deviceinfo.h"
#include "devicerecord.h"


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
    void resultReady(const USBRequestType type, const USBCStatus status);
    void loadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

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
    static constexpr unsigned SETTINGS_MAX_ID = settings_worker_t::maxID();

    using table_info_t = HIDTable<
        HIDTuple<uint32_t, DeviceInfo::time>,
        HIDTuple<uint32_t, DeviceInfo::min_id>,
        HIDTuple<uint32_t, DeviceInfo::max_id>,
        HIDTuple<uint32_t, DeviceInfo::current_id>,
        HIDTuple<uint32_t, DeviceInfo::current_count>,
        HIDTuple<uint8_t,  DeviceInfo::record_loaded>
    >;
    using info_worker_t = HIDTableWorker<table_info_t, SETTINGS_MAX_ID + 1>;
    static constexpr unsigned INFO_MAX_ID = info_worker_t::maxID();

    using table_record_t = HIDTable<
        HIDTuple<uint32_t, DeviceRecord::id>,
        HIDTuple<uint32_t, DeviceRecord::time>,
        HIDTuple<uint8_t,  DeviceRecord::ID,    __arr_len(DeviceRecord::record_t::ID)>,
        HIDTuple<uint16_t, DeviceRecord::value, __arr_len(DeviceRecord::record_t::value)>
    >;
    using record_worker_t = HIDTableWorker<table_record_t, INFO_MAX_ID + 1>;

    static settings_worker_t handlerSettings;
    static info_worker_t handlerInfo;
    static record_worker_t handlerRecord;

    USBCStatus loadLogProccess();

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
    void handleResults(const USBRequestType type, const USBCStatus status);
    void onLoadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

signals:
    void request(const USBRequestType type);
    void responseReady(const USBRequestType type, const USBCStatus status);
    void error(const QString& message);
    void loadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

};


#endif // USBWATCHER_H
