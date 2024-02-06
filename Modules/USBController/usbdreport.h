#ifndef USBREPORT_H
#define USBREPORT_H


#include <memory>
#include <cstdint>


struct USBDReport
{
    struct report_t {
        unsigned length;
        std::shared_ptr<uint8_t[]> instance;
    };

    static void setReport(std::shared_ptr<uint8_t[]> report, unsigned length);
    static report_t& getReport();

    static void setCharaсteristic(uint16_t characteristic);
    static uint16_t getCharaсteristic();

private:
    static report_t payload;
    static uint16_t characteristic;
};

#endif // USBREPORT_H
