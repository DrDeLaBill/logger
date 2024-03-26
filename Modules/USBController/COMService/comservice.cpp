#include "comservice.h"

#include <memory>
#include <cstdio>
#include <cstdint>

#include <QString>

#include "log.h"
#include "variables.h"

#include "com_defs.h"
#include "usbhreport.h"
#include "usbdreport.h"
#include "app_exception.h"


#define COM_REPORT_DELAY_MS (1000)


COMService::~COMService()
{
    if (port && port->isOpen()) {
        this->deinit();
    }
}

void COMService::init(const std::string& portName)
{
    port = std::make_unique<QSerialPort>();

    port->setPortName(QString(portName.c_str()));
    port->setBaudRate(12000000);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if(!port->open(QIODevice::ReadWrite)) {
        deinit();
        throw exceptions::UsbInitException();
    }
}

void COMService::deinit()
{
    if (!port) {
        return;
    }
    port->clear();

    if (!port->isOpen()) {
        return;
    }
    port->close();

    port.reset();
}

void COMService::sendReport(const report_pack_t& request)
{
    USBHReport::createReport(request);

    try {
        const QByteArray requestArray(reinterpret_cast<char*>(&(USBHReport::getReport())), sizeof(report_pack_t));
        qint64 res = port->write(requestArray);
        if (res != sizeof(report_pack_t)) {
            throw exceptions::UsbTimeoutException();
        }

        if (!port->waitForBytesWritten(COM_REPORT_DELAY_MS)) {
            throw exceptions::UsbTimeoutException();
        }

        if (!port->waitForReadyRead(COM_REPORT_DELAY_MS)) {
            throw exceptions::UsbTimeoutException();
        }

        QByteArray response;
        while (port->bytesAvailable()) {
            response += port->readAll();
            port->clear();
        }

        if (response.size() != sizeof(report_pack_t)) {
            throw exceptions::UsbReportException();
        }

        report_pack_t reportPack;
        uint8_t* reportPack_ptr = reinterpret_cast<uint8_t*>(&reportPack);
        for (unsigned i = 0; i < response.size(); i++) {
            reportPack_ptr[i] = response[i];
        }

        USBDReport::setReport(reportPack);

        port->clear();
    } catch (...) {
        throw;
    }
}
