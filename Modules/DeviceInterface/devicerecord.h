#ifndef DEVICEREPORT_H
#define DEVICEREPORT_H


#include <cstdint>

#include "devicedefs.h"


struct DeviceRecord
{
public:
    typedef struct __attribute__((packed)) _modbus_sensor_t {
        uint8_t  ID;
        uint16_t value;
    } modbus_sensor_t;

    typedef struct __attribute__((packed)) __1wire_sensor_t {
        uint64_t ADDR;
        uint16_t value;
    } _1wire_sensor_t;

    typedef struct __attribute__((packed)) _reocrd_t {
        uint32_t id;                                 // Record ID
        uint32_t time;                               // Record time
        modbus_sensor_t mb1_sens[MODBUS_SENS_COUNT]; // Record MODDBUS registers values
        _1wire_sensor_t ow_sens [MODBUS_SENS_COUNT]; // Record 1WIRE registers values
    } record_t;

protected:
    static record_t record;

public:
    struct rcrd_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct time
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct MODBUS1_ID
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct MODBUS1_value
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct _1WIRE_ADDR
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct _1WIRE_value
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
};


#endif // DEVICEREPORT_H
