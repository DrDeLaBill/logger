#include "usbdreport.h"

#include <cstring>

#include "Modules/Exceptions/app_exception.h"
#include "Modules/HIDTable/HIDController.h"


uint16_t USBDReport::characteristic = 0;
USBDReport::report_t USBDReport::payload{0, nullptr};


void USBDReport::setReport(std::shared_ptr<uint8_t[]> report, unsigned int length)
{
    if (report[0] != HID_OUTPUT_REPORT_ID) {
        throw new exceptions::UsbReportException();
    }
    int roffset = length - strlen(REPORT_PREFIX);
    if (roffset - 1 <= 0) {
        throw new exceptions::UsbReportException();
    }
    if (memcmp(&report[roffset], REPORT_PREFIX, strlen(REPORT_PREFIX))) {
        throw new exceptions::UsbReportException();
    }
    USBDReport::payload.length = roffset;
    USBDReport::payload.instance = std::shared_ptr<uint8_t[]>(new uint8_t[roffset], [] (uint8_t* arr) {
        delete [] arr;
    });
    memcpy(USBDReport::payload.instance.get(), &report[1], roffset);
}

USBDReport::report_t& USBDReport::getReport()
{
    return USBDReport::payload;
}

void USBDReport::setCharaсteristic(uint16_t characteristic)
{
    USBDReport::characteristic = characteristic;
}

uint16_t USBDReport::getCharaсteristic()
{
    return characteristic;
}
