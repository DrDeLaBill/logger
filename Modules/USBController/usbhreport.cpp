#include "usbhreport.h"

#include <cstring>


report_pack_t USBHReport::report{};


void USBHReport::createReport(const report_pack_t& report)
{
    memcpy(reinterpret_cast<void*>(&USBHReport::report), reinterpret_cast<void*>(const_cast<report_pack_t*>(&report)), sizeof(report));
    USBHReport::report.report_id = HID_INPUT_REPORT_ID;
    memcpy(USBHReport::report.tag, REPORT_PREFIX, sizeof(USBHReport::report.tag));
}

report_pack_t& USBHReport::getReport()
{
    return report;
}
