#ifndef ONEWIREDATA_H
#define ONEWIREDATA_H


#include <stdint.h>


struct OneWireData
{
    unsigned number;
    uint64_t address;
    int      value;
};


#endif // ONEWIREDATA_H
