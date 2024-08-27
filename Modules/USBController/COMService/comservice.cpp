#include "comservice.h"

#include <memory>
#include <cstdio>
#include <cstdint>
#include <iostream>

#include <QString>

#include "glog.h"
#include "variables.h"

#include "greport.h"
#include "usbhreport.h"
#include "usbdreport.h"
#include "app_exception.h"


#define COM_REPORT_DELAY_MS (1000)


std::unique_ptr<QSerialPort> COMService::port;
std::string COMService::m_portName = "";


void COMService::begin(const std::string& portName)
{
    if (port && port->isOpen()) {
        deinit();
    }

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

    m_portName = portName;
}

void COMService::deinit()
{
    m_portName = "";

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

void COMService::sendReport(const pack_t& request)
{
    USBHReport::createReport(request);

    try {
        // printPretty("Reports:\n");
        // com_report_show(&(USBHReport::getReport()));

        const QByteArray requestArray(reinterpret_cast<char*>(&(USBHReport::getReport())), sizeof(pack_t));
        qint64 res = port->write(requestArray);
        if (res != sizeof(pack_t)) {
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

        if (response.size() != sizeof(pack_t)) {
            throw exceptions::UsbReportException();
        }

        pack_t reportPack;
        uint8_t* reportPack_ptr = reinterpret_cast<uint8_t*>(&reportPack);
        for (unsigned i = 0; i < response.size(); i++) {
            reportPack_ptr[i] = response[i];
        }

        USBDReport::setReport(reportPack);

        port->clear();

        // com_report_show(&(USBDReport::getReport()));
    } catch (...) {
        throw;
    }
}

bool COMService::available()
{
    if (!port) {
        return false;
    }
    return port->isOpen();
}
