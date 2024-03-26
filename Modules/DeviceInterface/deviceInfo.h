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
        uint32_t current_count;
        uint8_t  record_loaded;
        // MODBUS 1 sensor register last values
        uint16_t modbus1_value[MODBUS_SENS_COUNT];
    } info_t;

protected:
    static info_t info;

public:
    struct time
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct min_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct max_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct current_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct current_count
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct record_loaded
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct modbus1_value
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
};

#endif // DEVICEINFO_H
