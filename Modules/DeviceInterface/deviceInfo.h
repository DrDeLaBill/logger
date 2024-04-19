#ifndef DEVICEINFO_H
#define DEVICEINFO_H


#include <cstdint>

#include "devicedefs.h"


class DeviceInfo
{
public:
    typedef struct _info_t {
        uint32_t time;
        uint32_t min_id;
        uint32_t max_id;
        uint32_t current_id;
        uint64_t current_mbodbus1_count;
        uint64_t current_1wire_count;
        uint8_t  record_loaded;
        bool     need_registrate_1wire;
        // MODBUS 1 sensor register last values
        int16_t modbus1_last_value[MODBUS_SENS_COUNT];
        // 1WIRE sensor register last values
        int16_t _1wire_last_value[MODBUS_SENS_COUNT];
    } info_t;

protected:
    static info_t info;

public:
    struct time
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct min_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct max_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct current_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct current_mbodbus1_count
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct current_1wire_count
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct need_registrate_1wire
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct record_loaded
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };
    struct modbus1_last_value
    {
        static uint16_t ID;
        static bool* updated;
        static void set(int16_t value, unsigned index = 0);
        static int16_t get(unsigned index = 0);
    };
    struct _1wire_last_value
    {
        static uint16_t ID;
        static bool* updated;
        static void set(int16_t value, unsigned index = 0);
        static int16_t get(unsigned index = 0);
    };
    struct _1wire_registrate
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint64_t value, unsigned index = 0);
        static uint64_t get(unsigned index = 0);
    };

};

#endif // DEVICEINFO_H
