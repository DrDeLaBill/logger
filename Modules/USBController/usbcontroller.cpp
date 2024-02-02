#include "usbcontroller.h"

#include <iostream>

#include <cstdio>

#include <libusb.h>

#include "mainwindow.h"
#include "hidservice.h"
#include "app_exception.h"
#include "USB_HID_Table.h"


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

void USBController::search()
{
    emit operate("search");
}

void USBController::handleResults(const QString &parameter)
{
    std::cout << "SUCCESS" << std::endl;
    std::cout << parameter.toStdString() << std::endl;
    MainWindow::setBytesLabel("SUCCESS");
}

void USBWorker::doWork(const QString &parameter)
{
    std::cout << parameter.toStdString() << std::endl;

    try {
        HIDService::init();

        while (!HIDService::isDeviceConnected(HID_VENDOR_ID, HID_PRODUCT_ID));

        HIDService::deinit();

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
