#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H


#include <cstdint>

#include "utils.h"

#include "devicedefs.h"


typedef enum _sensor_status_t {
    SETTINGS_SENSOR_EMPTY    = (uint16_t)0x0000,
    SETTINGS_SENSOR_THERMAL  = (uint16_t)0x0001,
    SETTINGS_SENSOR_HUMIDITY = (uint16_t)0x0002,
    SETTINGS_SENSOR_ANOTHER  = (uint16_t)0x4000,
    SETTINGS_SENSOR_ERROR    = (uint16_t)0x8000,
} sensor_status_t;


struct DeviceSettings
{
public:
    typedef struct _settings_t {
        // Device type
        uint16_t  dv_type;
        // Software version
        uint8_t  sw_id;
        // Firmware version
        uint8_t  fw_id;
        // Configuration version
        uint32_t cf_id;
        // Log record period time in ms
        uint32_t record_period;
        // Send record period time in ms
        uint32_t send_period;
        // Last sended record ID
        uint32_t record_id; // TODO: load record without saving settings

        // The ID sensor on the bus is the N-1 index in the arrays
        // MODBUS 1 sensors statuses
        uint16_t modbus1_status   [MODBUS_SENS_COUNT];
        // MODBUS 1 sensor register IDs for reading values
        uint16_t modbus1_value_reg[MODBUS_SENS_COUNT];
        // MODBUS 1 sensor register IDs for setting new sensor ids
        uint16_t modbus1_id_reg   [MODBUS_SENS_COUNT];

        // Device time
        uint32_t time;
    } settings_t;

protected:
    static settings_t settings;

public:
    static bool check();

    struct dv_type
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct sw_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct fw_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct cf_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct record_period
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct send_period
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct record_id
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct modbus1_status
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct modbus1_value_reg
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };
    struct modbus1_id_reg
    {
        static uint16_t ID;
        static bool* updated;
        static void set(uint32_t value, unsigned index = 0);
        static uint32_t get(unsigned index = 0);
    };

    static unsigned getIndex(const unsigned index = 0);

    static void clear();
};

#endif // DEVICESETTINGS_H
