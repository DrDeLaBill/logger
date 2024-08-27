#include "usbcontroller.h"

#include <fstream>
#include <iostream>

#include <cstdio>

#include "app.h"
#include "glog.h"
#include "settings.h"

#include "mainwindow.h"
#include "comservice.h"
#include "app_exception.h"
#include "CodeStopwatch.h"


USBRequestType USBController::requestType = USB_REQUEST_NONE;
TableWorker USBWorker::tableWorker;


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
    memset(reinterpret_cast<void*>(&settings), 0, sizeof(settings));
    memset(reinterpret_cast<void*>(&app_info), 0, sizeof(app_info));
    emit request(requestType, port);
}

void USBController::saveSettings(const QString& port)
{
    requestType = USB_REQUEST_SAVE_SETTINGS;
    emit request(requestType, port);
}

void USBController::handleResults(const USBRequestType type, const USBCStatus status)
{
    if (status != USBC_RES_OK && status != USBC_RES_DONE) {
        COMService::deinit();
        emit error(exceptions::USBExceptionGroup().message);
    }

    if (requestType == type) {
        requestType = USB_REQUEST_NONE;
    }
    emit responseReady(type, status);
}

void USBWorker::proccess(const USBRequestType type, const QString& port)
{
    USBCStatus status = USBC_RES_OK;

    try {
        if (!COMService::available()) {
            COMService::deinit();
            COMService::begin(port.toStdString());
        }

        switch (type) {
        case USB_REQUEST_LOAD_SETTINGS:
            status = tableWorker.load();
            break;
        case USB_REQUEST_SAVE_SETTINGS:
            status = tableWorker.save();
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

        COMService::deinit();
    } catch (const exceptions::ExceptionBase* exc) {
        if (exc->groupMessage() == exceptions::USBExceptionGroup().message) {
            status = USBC_RES_ERROR;
        } else {
            status = USBC_INTERNAL_ERROR;
        }

        COMService::deinit();
    } catch (const std::exception* exc) {
        status = USBC_INTERNAL_ERROR;

        COMService::deinit();
    }

    emit resultReady(type, status);
}
