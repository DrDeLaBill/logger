#include "usbcontroller.h"

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
    while (curLogId <= DeviceInfo::max_id::get()) {
        USBCStatus status = handlerInfo.load();
        if (status != USBC_RES_DONE) {
            throw exceptions::UsbReportException();
        }

        if (DeviceInfo::record_loaded::get() == 0) {
            continue;
        }
        curLogId = DeviceInfo::current_id::get();

        status = handlerRecord.load();
        if (status != USBC_RES_DONE) {
            throw exceptions::UsbReportException();
        }

        printPretty("##########RECORD##########\n");
        printPretty("Record ID: %lu\n", DeviceRecord::id::get());
        printPretty("Record time: %lu\n", DeviceRecord::time::get());
        printPretty("INDEX\tID\tVALUE\n");
        for (uint8_t i = 0; i < DeviceInfo::current_count::get(); i++) {
            printPretty("%03u\t%03u\t%u\n", i, DeviceRecord::ID::get(i), DeviceRecord::value::get(i));
        }
        if (!DeviceInfo::current_count::get()) {
            printPretty("----------EMPTY-----------\n");
        }
        printPretty("##########RECORD##########\n");

        DeviceInfo::current_id::set(curLogId);
        status = handlerInfo.save();
        if (status != USBC_RES_DONE) {
            throw exceptions::UsbReportException();
        }
    }

    return USBC_RES_DONE;
}
