#ifndef USBHREPORT_H
#define USBHREPORT_H


#include <cstdint>

#include "hid_defs.h"


struct USBHReport
{
public:
    static void createReport(const report_pack_t& report);
    static report_pack_t& getReport();

private:
    static report_pack_t report;
};

#endif // USBHREPORT_H
