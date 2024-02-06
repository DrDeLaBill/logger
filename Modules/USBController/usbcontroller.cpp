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
    // USB HID device check
    QObject::connect(this, &USBController::operate, worker, &USBWorker::doWork);
    QObject::connect(worker, &USBWorker::resultReady, this, &USBController::handleResults);
    // Read characteristics
    QObject::connect(this, &USBController::requestCharacteristic, worker, &USBWorker::readCharacteristic);
    QObject::connect(worker, &USBWorker::characteristicReady, this, &USBController::responseCharacteristic);
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
    std::cout << "doWork response: " << parameter.toStdString() << std::endl;

    if (parameter == curParameter) {
        MainWindow::setBytesLabel(parameter);
    } else {
        MainWindow::setError(parameter);
    }

    if (parameter == USB_SEARCH_HANDLER || parameter == USB_REPORT_HANDLER) { // TODO: remove USB_REPORT_HANDLER
        proccess(USB_REPORT_HANDLER);
    } else if (parameter == exceptions::USBExceptionGroup().message) {
        proccess(USB_SEARCH_HANDLER);
    }
}

void USBController::responseCharacteristic(const uint16_t key, const uint8_t* result, const uint8_t index)
{
    std::cout << "readCharacteristic response: " << key << "[" << index << "] = " << result[0] << std::endl;

    hid_controller.setValue(key, result, index);

    // TODO: emit next charctrstc
}

void USBWorker::doWork(const QString& parameter)
{
    std::cout << "doWork request : " << parameter.toStdString() << std::endl;

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

void USBWorker::readCharacteristic(const uint16_t key, const uint8_t index)
{
    std::cout << "readCharacteristic request : " << key << "[" << index << "]" << std::endl;

    // TODO: read charctrsts from USB
}
