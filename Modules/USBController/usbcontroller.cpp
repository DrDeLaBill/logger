#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "mainwindow.h"
#include "Modules/Exceptions/app_exception.h"


USBController::USBController(): curParameter("")
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
    curParameter = parameter;
    operate(parameter);
}

void USBController::handleResults(const QString& parameter)
{
    std::cout << "result: " << parameter.toStdString() << std::endl;

    if (parameter == curParameter) {
        MainWindow::setBytesLabel(parameter);
    } else {
        MainWindow::setError(parameter);
    }

    if (parameter == USB_SEARCH_HANDLER || parameter == USB_REPORT_HANDLER) {
        proccess(USB_REPORT_HANDLER);
    } else if (parameter == exceptions::USBExceptionGroup().message) {
        proccess(USB_SEARCH_HANDLER);
    }
}

void USBWorker::doWork(const QString& parameter)
{
    std::cout << "request: " << parameter.toStdString() << std::endl;

    try {
        auto handler = handlers.find(parameter);

        if (handler == handlers.end()) {
            throw new exceptions::UsbUndefinedBehaviourException(); // TODO: list of user's errors (errors that shows for user)
        }

        auto lambda = [&] (const auto& hndl) {
            hndl.operate();
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
