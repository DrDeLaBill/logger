#include "usbhreport.h"

#include <cstring>


report_pack_t USBHReport::report{};


void USBHReport::createReport(const report_pack_t& report)
{
    memcpy(reinterpret_cast<void*>(&USBHReport::report), reinterpret_cast<void*>(const_cast<report_pack_t*>(&report)), sizeof(report));
    USBHReport::report.flag = COM_SEND_FLAG;
    USBHReport::report.crc  = com_get_crc(&(USBHReport::report));
}

report_pack_t& USBHReport::getReport()
{
    return report;
}
