#ifndef USBREPORT_H
#define USBREPORT_H


#include <cstdint>

#include "greport.h"


struct USBDReport
{
public:
    static void setReport(const pack_t& report);
    static pack_t& getReport();

private:
    static pack_t report;
};

#endif // USBREPORT_H
