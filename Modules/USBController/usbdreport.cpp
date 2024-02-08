#include "usbdreport.h"

#include <cstring>

#include "app_exception.h"
//#include "HIDController.h" // TODO


//uint16_t USBDReport::characteristic_id = 0; // TODO
report_pack_t USBDReport::report = {};


void USBDReport::setReport(const report_pack_t& report)
{
    if (report.report_id != HID_OUTPUT_REPORT_ID) {
        throw new exceptions::UsbReportException();
    }
    if (memcmp(&report.tag, REPORT_PREFIX, sizeof(REPORT_PREFIX))) {
        throw new exceptions::UsbReportException();
    }
    memcpy(USBDReport::report.data, report.data, sizeof(report.data));
}

report_pack_t& USBDReport::getReport()
{
    return USBDReport::report;
}

//void USBDReport::setCharaсteristic(uint16_t characteristic_id) // TODO
//{
//    USBDReport::characteristic_id = characteristic_id;
//}

//uint16_t USBDReport::getCharaсteristic()
//{
//    return characteristic_id;
//}
