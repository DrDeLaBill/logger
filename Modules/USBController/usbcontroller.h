#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <cstdint>
#include <variant>
#include <type_traits>

#include <QThread>
#include <QObject>
#include <QString>

#include "utils.h"

#include "usbcstatus.h"
#include "comservice.h"
#include "COMController.h"
#include "comtableworker.h"

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
    void proccess(const USBRequestType type, const QString& port);

signals:
    void resultReady(const USBRequestType type, const USBCStatus status);
    void loadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

private:
    static constexpr char TAG[] = "USBW";

    using table_settings_t = COMTable<
        COMTuple<uint16_t, DeviceSettings::dv_type>,
        COMTuple<uint8_t,  DeviceSettings::sw_id>,
        COMTuple<uint8_t,  DeviceSettings::fw_id>,
        COMTuple<uint32_t, DeviceSettings::cf_id>,
        COMTuple<uint32_t, DeviceSettings::record_period>,
        COMTuple<uint32_t, DeviceSettings::send_period>,
        COMTuple<uint32_t, DeviceSettings::record_id>,
        COMTuple<uint16_t, DeviceSettings::modbus1_status,    __arr_len(DeviceSettings::settings_t::modbus1_status)>,
        COMTuple<uint16_t, DeviceSettings::modbus1_value_reg, __arr_len(DeviceSettings::settings_t::modbus1_value_reg)>,
        COMTuple<uint16_t, DeviceSettings::modbus1_id_reg,    __arr_len(DeviceSettings::settings_t::modbus1_id_reg)>,
        COMTuple<uint64_t, DeviceSettings::_1wire_address,    __arr_len(DeviceSettings::settings_t::_1wire_address)>,
        COMTuple<uint8_t,  DeviceSettings::mb1_last_id>,
        COMTuple<uint8_t,  DeviceSettings::mb1_new_id>,
        COMTuple<uint8_t,  DeviceSettings::need_mb1_id_update>
    >;
    using settings_worker_t = COMTableWorker<table_settings_t, COM_FIRST_KEY>;
    static constexpr unsigned SETTINGS_MAX_ID = settings_worker_t::maxID();

    using table_info_t = COMTable<
        COMTuple<uint32_t, DeviceInfo::time>,
        COMTuple<uint32_t, DeviceInfo::min_id>,
        COMTuple<uint32_t, DeviceInfo::max_id>,
        COMTuple<uint32_t, DeviceInfo::current_id>,
        COMTuple<uint8_t,  DeviceInfo::next_record>,
        COMTuple<uint8_t,  DeviceInfo::current_mbodbus1_count>,
        COMTuple<uint8_t,  DeviceInfo::current_1wire_count>,
        COMTuple<uint8_t,  DeviceInfo::need_registrate_1wire>,
        COMTuple<uint8_t,  DeviceInfo::record_loaded>,
        COMTuple<int16_t,  DeviceInfo::modbus1_last_value, __arr_len(DeviceInfo::info_t::modbus1_last_value)>,
        COMTuple<int16_t,  DeviceInfo::_1wire_last_value,  __arr_len(DeviceInfo::info_t::_1wire_last_value)>,
        COMTuple<uint64_t, DeviceInfo::_1wire_registrate,  __arr_len(DeviceRecord::record_t::ow_sens)>
    >;
    using info_worker_t = COMTableWorker<table_info_t, SETTINGS_MAX_ID + 1>;
    static constexpr unsigned INFO_MAX_ID = info_worker_t::maxID();

    using table_record_t = COMTable<
        COMTuple<uint32_t, DeviceRecord::rcrd_id>,
        COMTuple<uint32_t, DeviceRecord::time>,
        COMTuple<uint8_t,  DeviceRecord::MODBUS1_ID,    __arr_len(DeviceRecord::record_t::mb1_sens)>,
        COMTuple<int16_t,  DeviceRecord::MODBUS1_value, __arr_len(DeviceRecord::record_t::mb1_sens)>,
        COMTuple<uint64_t, DeviceRecord::_1WIRE_ADDR,   __arr_len(DeviceRecord::record_t::ow_sens)>,
        COMTuple<int16_t,  DeviceRecord::_1WIRE_value,  __arr_len(DeviceRecord::record_t::ow_sens)>
    >;
    using record_worker_t = COMTableWorker<table_record_t, INFO_MAX_ID + 1>;

    static settings_worker_t handlerSettings;
    static info_worker_t handlerInfo;
    static record_worker_t handlerRecord;

    USBCStatus loadLogProccess(const COMService& comService);

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

    void loadSettings(const QString& port);
    void saveSettings(const QString& port);
    void loadInfo(const QString& port);
    void saveInfo(const QString& port);
    void loadLog(const QString& port);


public slots:
    void handleResults(const USBRequestType type, const USBCStatus status);
    void onLoadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

signals:
    void request(const USBRequestType type, const QString& port);
    void responseReady(const USBRequestType type, const USBCStatus status);
    void error(const QString& message);
    void loadLogProgressUpdated(uint32_t value); // TODO: USBWorker -> MainWindow

};


#endif // USBWATCHER_H
