#include "usbdreport.h"

#include <cstring>

#include "app_exception.h"


report_pack_t USBDReport::report = {};


void USBDReport::setReport(const report_pack_t& report)
{
    if (report.flag != COM_SEND_FLAG) {
        throw new exceptions::UsbReportException();
    }
    memcpy(reinterpret_cast<void*>(&(USBDReport::report)), reinterpret_cast<void*>(const_cast<report_pack_t*>(&report)), sizeof(report));
}

report_pack_t& USBDReport::getReport()
{
    return USBDReport::report;
}
