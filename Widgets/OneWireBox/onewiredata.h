#ifndef ONEWIREDATA_H
#define ONEWIREDATA_H


#include <stdint.h>


struct OneWireData
{
    unsigned number;
    uint64_t address;
    uint16_t value;
};


#endif // ONEWIREDATA_H
