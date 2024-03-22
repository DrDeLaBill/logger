#include "comservice.h"

#include <memory>
#include <cstdio>
#include <cstdint>

#include <QString>

#include "log.h"
#include "Timer.h"
#include "variables.h"

#include "com_defs.h"
#include "usbhreport.h"
#include "usbdreport.h"
#include "app_exception.h"


#define RESPONSE_DELAY_MS (1000)


void COMService::init(const std::string& portName)
{
    port = std::make_unique<QSerialPort>();

    port->setPortName(QString(portName.c_str()));
    port->setBaudRate(115200);
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
    port->clear();
    port->close();

    port.reset();
}

void COMService::sendReport(const report_pack_t& request)
{
    USBHReport::createReport(request);

    try {
        port->write(reinterpret_cast<char*>(&(USBHReport::getReport())));

        if (!port->waitForBytesWritten(30000)) {
            throw exceptions::UsbTimeoutException();
        }

        utl::Timer timer(RESPONSE_DELAY_MS);
        timer.start();

        while (!port->bytesAvailable()) {
            if (!timer.wait()) {
                throw exceptions::UsbTimeoutException();
            }
        }

        QByteArray response;
        while (port->bytesAvailable()) {
            response += port->readAll();
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
    } catch (...) {
        deinit();
        throw;
    }
}
