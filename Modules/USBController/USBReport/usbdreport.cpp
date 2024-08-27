#include "usbdreport.h"

#include <cstring>

#include "app_exception.h"


pack_t USBDReport::report = {};


void USBDReport::setReport(const pack_t& report)
{
    memcpy(reinterpret_cast<void*>(&(USBDReport::report)), reinterpret_cast<void*>(const_cast<pack_t*>(&report)), sizeof(report));
}

pack_t& USBDReport::getReport()
{
    return USBDReport::report;
}
