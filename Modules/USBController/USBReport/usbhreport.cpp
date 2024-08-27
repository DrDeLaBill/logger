#include "usbhreport.h"

#include <cstring>


pack_t USBHReport::report{};


void USBHReport::createReport(const pack_t& report)
{
    memcpy(reinterpret_cast<void*>(&USBHReport::report), reinterpret_cast<void*>(const_cast<pack_t*>(&report)), sizeof(report));
}

pack_t& USBHReport::getReport()
{
    return report;
}
