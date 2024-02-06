#ifndef HIDSTRUCT_H
#define HIDSTRUCT_H


#include <cstdint>


struct __attribute__((packed)) HIDStruct
{
    uint16_t key;
    uint8_t index;
    uint8_t value[sizeof(uint32_t)];
};


#endif // HIDSTRUCT_H
