#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "log.h"
#include "mainwindow.h"
#include "app_exception.h"


USBController::USBController(): curParameter("")
{
    USBWorker *worker = new USBWorker;
    worker->moveToThread(&workerThread);

    QObject::connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    // USB HID device check
    // Read characteristics
    QObject::connect(this, &USBController::operate, worker, &USBWorker::doWork);
    QObject::connect(worker, &USBWorker::resultReady, this, &USBController::handleResults);
    // Write characteristics
    // TODO ---...---

    workerThread.start();
}

USBController::~USBController()
{
    workerThread.quit();
    workerThread.wait();
}

void USBController::proccess(const QString& parameter)
{
    curParameter = parameter;
    emit operate(parameter);
}

void USBController::handleResults(const QString& parameter)
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "response: %s", parameter.toStdString().c_str());
#endif

    if (parameter == curParameter) {
        MainWindow::setBytesLabel(parameter);
    } else {
        MainWindow::setError(parameter);
        proccess(USB_SEARCH_HANDLER); // TODO: do normal if
        return;
    }

    if (parameter == USB_SEARCH_HANDLER || parameter == USB_GET_CHARACTERISTIC_HANDLER) { // TODO: remove USB_REPORT_HANDLER
        proccess(USB_GET_CHARACTERISTIC_HANDLER);
    } else if (parameter == USB_LOAD_RECORD_HANDLER) {
        proccess(USB_LOAD_RECORD_HANDLER);
    } else if (parameter == exceptions::USBExceptionGroup().message) {
        proccess(USB_SEARCH_HANDLER);
    }
}

void USBWorker::doWork(const QString& parameter)
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "request : %s", parameter.toStdString().c_str());
#endif

    try {
        auto handler = handlers.find(parameter);

        if (handler == handlers.end()) {
            throw new exceptions::UsbUndefinedBehaviourException(); // TODO: list of user's errors (errors that shows for user)
        }

        auto lambda = [] (const auto& handler) {
            handler.operate();
        };

        std::visit(lambda, handler->second);

        emit resultReady(parameter);
    } catch (const exceptions::ExceptionBase& exc) {
        // TODO: log error -> exc.what()
        emit resultReady(exc.groupMessage());
    } catch (const exceptions::ExceptionBase* exc) {
        emit resultReady(exc->groupMessage());
    } catch (...) {
        emit resultReady(exceptions::InternalErrorException().groupMessage());
    }
}
