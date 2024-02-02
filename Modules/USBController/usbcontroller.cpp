#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "mainwindow.h"
#include "Modules/Exceptions/app_exception.h"


USBController::USBController()
{
    USBWorker *worker = new USBWorker;
    worker->moveToThread(&workerThread);
    QObject::connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(this, &USBController::operate, worker, &USBWorker::doWork);
    QObject::connect(worker, &USBWorker::resultReady, this, &USBController::handleResults);
    workerThread.start();
}

USBController::~USBController()
{
    workerThread.quit();
    workerThread.wait();
}

void USBController::proccess(const QString& parameter)
{
    operate(parameter);
}

void USBController::handleResults(const QString& parameter)
{
    MainWindow::setBytesLabel("SUCCESS");
}

void USBWorker::doWork(const QString& parameter)
{
    std::cout << parameter.toStdString() << std::endl;

    try {
        auto handler = handlers.find(parameter);

        if (handler == handlers.end()) {
            throw new exceptions::UsbUndefinedBehaviourException("INTERNAL_ERROR"); // TODO: list of user's errors (errors that shows for user)
        }

        auto lambda = [&] (auto& hndl) {
            hndl.operate();
        };

        std::visit(lambda, handler->second);

        emit resultReady(parameter);
    } catch (const exceptions::ExceptionBase& exc) {
        // TODO: alert and crash
        throw new exceptions::ExceptionBase("лох"); // TODO: normal message
        emit resultReady("USB_CONNECT_ERROR");
    } catch (const exceptions::ExceptionBase* exc) {
        // TODO: alert and crash
        throw new exceptions::ExceptionBase("лох"); // TODO: normal message
        emit resultReady("USB_CONNECT_ERROR");
    } catch (...) {
        throw new exceptions::ExceptionBase("неизвестный лох"); // TODO: normal message
        emit resultReady("INTERNAL_ERROR");
    }
}
