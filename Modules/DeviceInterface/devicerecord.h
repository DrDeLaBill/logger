#ifndef DEVICEREPORT_H
#define DEVICEREPORT_H


#include <cstdint>

#include "devicedefs.h"


struct DeviceRecord
{
public:
    typedef struct _reocrd_t {
        uint32_t id;
        uint32_t time;
        uint8_t  ID[MODBUS_SENS_COUNT];
        uint16_t value[MODBUS_SENS_COUNT];
    } record_t;

protected:
    static record_t record;

public:
    struct id
    {
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct time
    {
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct ID
    {
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct value
    {
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
};


#endif // DEVICEREPORT_H
