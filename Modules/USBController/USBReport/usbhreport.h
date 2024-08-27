#ifndef USBHREPORT_H
#define USBHREPORT_H


#include <cstdint>

#include "greport.h"


struct USBHReport
{
public:
    static void createReport(const pack_t& report);
    static pack_t& getReport();

private:
    static pack_t report;
};

#endif // USBHREPORT_H
