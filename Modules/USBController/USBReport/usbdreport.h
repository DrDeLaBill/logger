#ifndef USBREPORT_H
#define USBREPORT_H


#include <cstdint>

#include "com_defs.h"


struct USBDReport
{
public:
    static void setReport(const report_pack_t& report);
    static report_pack_t& getReport();

private:
    static report_pack_t report;
};

#endif // USBREPORT_H
