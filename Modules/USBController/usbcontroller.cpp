#include "usbcontroller.h"

#include <fstream>
#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "log.h" // TODO

#include "mainwindow.h"
#include "deviceinfo.h"
#include "devicerecord.h"
#include "app_exception.h"
#include "CodeStopwatch.h"


USBRequestType USBController::requestType = USB_REQUEST_NONE;
USBWorker::settings_worker_t USBWorker::handlerSettings;
USBWorker::info_worker_t USBWorker::handlerInfo;
USBWorker::record_worker_t USBWorker::handlerRecord;


USBController::USBController(): worker()
{
    worker.moveToThread(&workerThread);

    QObject::connect(&workerThread, &QThread::finished, &worker, &QObject::deleteLater);
    // USB COM device check
    // Read characteristics
    QObject::connect(this, &USBController::request, &worker, &USBWorker::proccess);
    QObject::connect(&worker, &USBWorker::resultReady, this, &USBController::handleResults);
    // Write characteristics
    // TODO ---...---

    QObject::connect(&worker, &worker.loadLogProgressUpdated, this, onLoadLogProgressUpdated);

    workerThread.start();
}

USBController::~USBController()
{
    workerThread.quit();
    workerThread.wait();
}

void USBController::loadSettings(const QString& port)
{
    requestType = USB_REQUEST_LOAD_SETTINGS;
    DeviceSettings::clear();
    emit request(requestType, port);
}

void USBController::saveSettings(const QString& port)
{
    requestType = USB_REQUEST_SAVE_SETTINGS;
    emit request(requestType, port);
}

void USBController::loadInfo(const QString& port)
{
    requestType = USB_REQUEST_LOAD_INFO;
    emit request(requestType, port);
}

void USBController::saveInfo(const QString& port)
{
    requestType = USB_REQUEST_SAVE_INFO;
    emit request(requestType, port);
}

void USBController::loadLog(const QString& port)
{
    requestType = USB_REQUEST_LOAD_LOG;
    emit request(requestType, port);
}

void USBController::handleResults(const USBRequestType type, const USBCStatus status)
{
    if (status != USBC_RES_OK && status != USBC_RES_DONE) {
        emit error(exceptions::USBExceptionGroup().message);
    }

    if (requestType == type) {
        requestType = USB_REQUEST_NONE;
    }
    emit responseReady(type, status);
}

void USBController::onLoadLogProgressUpdated(uint32_t value)
{
    emit loadLogProgressUpdated(value);
}

void USBWorker::proccess(const USBRequestType type, const QString& port)
{
    USBCStatus status = USBC_RES_OK;

    try {
        switch (type) {
        case USB_REQUEST_LOAD_SETTINGS:
            status = handlerSettings.load(port.toStdString());
            break;
        case USB_REQUEST_SAVE_SETTINGS:
            status = handlerSettings.save(port.toStdString());
            break;
        case USB_REQUEST_LOAD_INFO:
            status = handlerInfo.load(port.toStdString());
            break;
        case USB_REQUEST_SAVE_INFO:
            status = handlerInfo.save(port.toStdString());
            break;
        case USB_REQUEST_LOAD_LOG:
            status = loadLogProccess(port.toStdString());
            break;
        default:
            throw exceptions::UsbUndefinedBehaviourException();
        }
    } catch (const exceptions::ExceptionBase& exc) {
        if (exc.groupMessage() == exceptions::USBExceptionGroup().message) {
            status = USBC_RES_ERROR;
        } else {
            status = USBC_INTERNAL_ERROR;
        }
    } catch (const exceptions::ExceptionBase* exc) {
        if (exc->groupMessage() == exceptions::USBExceptionGroup().message) {
            status = USBC_RES_ERROR;
        } else {
            status = USBC_INTERNAL_ERROR;
        }
    } catch (...) {
        status = USBC_INTERNAL_ERROR;
    }

    emit resultReady(type, status);
}

USBCStatus USBWorker::loadLogProccess(const std::string& port)
{
    emit loadLogProgressUpdated(0);

    uint32_t curLogId = 0;
    USBCStatus status = USBC_RES_OK;

    DeviceInfo::current_id::set(curLogId);
    DeviceInfo::current_id::updated[0] = true;
    if (handlerInfo.save(port) != USBC_RES_DONE) {
        while (handlerInfo.loadCharacteristic(port, DeviceInfo::current_id::ID) != USBC_RES_DONE);
    }
    DeviceInfo::record_loaded::set(0);
    DeviceInfo::record_loaded::updated[0] = true;
    if (handlerInfo.save(port) != USBC_RES_DONE) {
        while (handlerInfo.loadCharacteristic(port, DeviceInfo::record_loaded::ID) != USBC_RES_DONE);
    }

    while (handlerInfo.load(port) != USBC_RES_DONE);

    std::string dumpStr = "";
    while (curLogId <= DeviceInfo::max_id::get()) {
        status = handlerInfo.loadCharacteristic(port, DeviceInfo::record_loaded::ID);
        if (status != USBC_RES_DONE) {
            continue;
        }
        status = handlerInfo.loadCharacteristic(port, DeviceInfo::current_id::ID);
        if (status != USBC_RES_DONE) {
            continue;
        }

        if (DeviceInfo::record_loaded::get() == 0) {
            continue;
        }
        uint32_t tmpId = DeviceInfo::current_id::get();
        if (curLogId < tmpId) {
            curLogId = tmpId;
        }

        status = handlerRecord.load(port);
        if (status != USBC_RES_DONE) {
            throw exceptions::UsbReportException();
        }

        for (uint8_t i = 0; i < DeviceInfo::current_count::get(); i++) {
            if (i == 0) {
                dumpStr += std::to_string(DeviceRecord::rcrd_id::get()) + "," + std::to_string(DeviceRecord::time::get()) + ",";
            } else {
                dumpStr += ",,";
            }
            dumpStr += std::to_string(DeviceRecord::snsr_id::get(i)) + "," + std::to_string(DeviceRecord::value::get(i)) + ",\n";
        }

        emit loadLogProgressUpdated(curLogId);

        DeviceInfo::record_loaded::set(0);
        DeviceInfo::record_loaded::updated[0] = true;
        DeviceInfo::current_id::set(curLogId);
        DeviceInfo::current_id::updated[0] = true;
        while (handlerInfo.save(port) != USBC_RES_DONE) {
            while (handlerInfo.loadCharacteristic(port, DeviceInfo::record_loaded::ID) != USBC_RES_DONE);
            while (handlerInfo.loadCharacteristic(port, DeviceInfo::current_id::ID) != USBC_RES_DONE);
        }

        curLogId++;
    }

    std::ofstream dump;
    dump.open(
        std::string("dump") +
        std::to_string(getMillis()) +
        std::string(".csv")
    );
    dump << "LOG ID,TIME,ID,VALUE,\n";
    dump << dumpStr.c_str();
    dump.close();

    return USBC_RES_DONE;
}
