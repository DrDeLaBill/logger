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


std::unique_ptr<QSerialPort> COMService::port;


void COMService::init(QString portName)
{

}

void COMService::deinit()
{

}

bool COMService::isDeviceConnected(uint16_t vendorId, uint16_t productId)
{
    return checkDevice(vendorId, productId);
}

void COMService::sendReport(const report_pack_t& request)
{
    USBHReport::createReport(request);

    int resLength = 0;
    try {





//        int res = libusb_interrupt_transfer(
//            handle,
//            COM_OUTPUT_ENDPOINT,
//            reinterpret_cast<uint8_t*>(&USBHReport::getReport()),
//            dev_desc.bLength,
//            &resLength,
//            COM_DELAY_MS
//        );
//        if (res < 0) {
//            // TODO: timeout for timeouts (if device has another version)
//            throw exceptions::UsbTimeoutException();
//        }
//        if (resLength != sizeof(request)) {
//            throw exceptions::UsbReportException();
//        }

//        report_pack_t response = {};
//        res = libusb_interrupt_transfer(
//            handle,
//            COM_INPUT_ENDPOINT,
//            reinterpret_cast<uint8_t*>(&response),
//            dev_desc.bLength,
//            &resLength,
//            COM_DELAY_MS
//        );
        if (res < 0) {
            // TODO: timeout for timeouts (if device has another version)
            throw exceptions::UsbTimeoutException();
        }

        try {
            USBDReport::setReport(response);
        } catch (...) {
            throw;
        }

        if (resLength == 0) {
            throw exceptions::UsbReportException();
        }
    } catch (...) {
        throw;
    }
}
