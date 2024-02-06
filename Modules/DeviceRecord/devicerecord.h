#ifndef DEVICEREPORT_H
#define DEVICEREPORT_H


#include <cstdint>

#include "Modules/DeviceSettings/devicesettings.h"


struct DeviceRecord
{
    struct record_t {
        // Record ID
        uint32_t id;
        // Record time
        uint32_t time;
        // Record registers values
        uint8_t  IDs[MODBUS_SENS_COUNT];
        uint16_t values[MODBUS_SENS_COUNT];
    };

    static record_t record;

    struct id     { uint32_t* operator()() { return &record.id; } };
    struct time   { uint32_t* operator()() { return &record.time; } };
    struct IDs    { uint8_t*  operator()() { return record.IDs; } };
    struct values { uint16_t* operator()() { return record.values; } };
};


#endif // DEVICEREPORT_H
