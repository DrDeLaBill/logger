#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "mainwindow.h"
#include "app_exception.h"
#include "CodeStopwatch.h"


USBRequestType USBController::requestType = USB_REQUEST_NONE;
USBWorker::settings_worker_t USBWorker::handlerSettings;
USBWorker::info_worker_t USBWorker::handlerInfo;


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

void USBController::handleResults(const USBCStatus status)
{
    if (status != USBC_RES_OK && status != USBC_RES_DONE) {
        MainWindow::setError(exceptions::USBExceptionGroup().message);
    }

    MainWindow::responseProccess(requestType, status);
    requestType = USB_REQUEST_NONE;
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
            status = USBC_INTERNAL_ERROR;
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

    emit resultReady(status);
}
