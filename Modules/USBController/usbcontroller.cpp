#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "log.h"
#include "mainwindow.h"
#include "app_exception.h"


USBController::USBController()
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

void USBController::proccess(const QString& handler)
{
    emit operate(handler);
}

void USBController::handleResults(const USBCStatus& status)
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "response: %u", status);
#endif

    MainWindow::proccess(status);
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

        USBCStatus status = USBC_RES_ERROR;

        auto lambda = [&] (const auto& handler) {
            status = handler.operate();
        };

        std::visit(lambda, handler->second);

        emit resultReady(status);
    } catch (const exceptions::ExceptionBase& exc) {
        if (exc.groupMessage() == exceptions::USBExceptionGroup().message) {
            emit resultReady(USBC_RES_ERROR);
        } else {
            emit resultReady(USBC_INTERNAL_ERROR);
        }
    } catch (const exceptions::ExceptionBase* exc) {
        if (exc->groupMessage() == exceptions::USBExceptionGroup().message) {
            emit resultReady(USBC_RES_ERROR);
        } else {
            emit resultReady(USBC_INTERNAL_ERROR);
        }
    } catch (...) {
        emit resultReady(USBC_INTERNAL_ERROR);
        throw;
    }
}
