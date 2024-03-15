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
    // USB HID device check
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

void USBController::loadSettings()
{
    requestType = USB_REQUEST_LOAD_SETTINGS;
    DeviceSettings::clear();
    emit request(requestType);
}

void USBController::saveSettings()
{
    requestType = USB_REQUEST_SAVE_SETTINGS;
    emit request(requestType);
}

void USBController::loadInfo()
{
    requestType = USB_REQUEST_LOAD_INFO;
    emit request(requestType);
}

void USBController::saveInfo()
{
    requestType = USB_REQUEST_SAVE_INFO;
    emit request(requestType);
}

void USBController::loadLog()
{
    requestType = USB_REQUEST_LOAD_LOG;
    emit request(requestType);
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

void USBWorker::proccess(const USBRequestType type)
{
    USBCStatus status = USBC_RES_OK;

    try {
        switch (type) {
        case USB_REQUEST_LOAD_SETTINGS:
            status = handlerSettings.load();
            break;
        case USB_REQUEST_SAVE_SETTINGS:
            status = handlerSettings.save();
            break;
        case USB_REQUEST_LOAD_INFO:
            status = handlerInfo.load();
            break;
        case USB_REQUEST_SAVE_INFO:
            status = handlerInfo.save();
            break;
        case USB_REQUEST_LOAD_LOG:
            status = loadLogProccess();
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

USBCStatus USBWorker::loadLogProccess()
{
    uint32_t curLogId = DeviceInfo::min_id::get();
    USBCStatus status = USBC_RES_OK;

    DeviceInfo::record_loaded::set(0);
    DeviceInfo::record_loaded::updated[0] = true;
    if (handlerInfo.save() != USBC_RES_DONE) {
        while (handlerInfo.load() != USBC_RES_DONE);
    }
    DeviceInfo::current_id::set(curLogId);
    DeviceInfo::current_id::updated[0] = true;
    if (handlerInfo.save() != USBC_RES_DONE) {
        while (handlerInfo.load() != USBC_RES_DONE);
    }

    std::ofstream dump;
    dump.open("C:\\Users\\georg\\Documents\\dump.csv");
    dump << "LOG ID,TIME,ID,VALUE,\n";
    while (curLogId <= DeviceInfo::max_id::get()) {
        status = handlerInfo.load();
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

        status = handlerRecord.load();
        if (status != USBC_RES_DONE) {
            throw exceptions::UsbReportException();
        }

        for (uint8_t i = 0; i < DeviceInfo::current_count::get(); i++) {
            if (i == 0) {
                dump << DeviceRecord::id::get() << "," << DeviceRecord::time::get() << ",";
            } else {
                dump << ",,";
            }
            dump << DeviceRecord::ID::get(i) << "," << DeviceRecord::value::get(i) << ",\n";
        }
        emit loadLogProgressUpdated(curLogId);

        DeviceInfo::record_loaded::set(0);
        DeviceInfo::record_loaded::updated[0] = true;
        if (handlerInfo.save() != USBC_RES_DONE) {
            while (handlerInfo.load() != USBC_RES_DONE);
        }
        DeviceInfo::current_id::set(curLogId);
        DeviceInfo::current_id::updated[0] = true;
        if (handlerInfo.save() != USBC_RES_DONE) {
            while (handlerInfo.load() != USBC_RES_DONE);
        }
        curLogId++;
    }

    dump.close();

    return USBC_RES_DONE;
}
