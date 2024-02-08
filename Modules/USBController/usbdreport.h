#ifndef USBREPORT_H
#define USBREPORT_H


#include <cstdint>

#include "HIDStruct.h"


struct USBDReport
{
public:
    static void setReport(const report_pack_t& report);
    static report_pack_t& getReport();

//    static void setCharaсteristic(uint16_t characteristic_id); // TODO
//    static uint16_t getCharaсteristic();

private:
    static report_pack_t report;
//    static uint16_t characteristic_id;
};

#endif // USBREPORT_H
