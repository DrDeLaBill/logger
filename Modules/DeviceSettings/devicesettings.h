#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H


#include <cstdint>


#define DV_TYPE            ((uint16_t)0x0001)
#define SW_VERSION         ((uint8_t)0x04)
#define FW_VERSION         ((uint8_t)0x01)

#define MODBUS_SENS_COUNT  ((uint8_t)127)


struct DeviceSettings
{
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

    static settings_t settings;

    static bool check();

    struct dv_type
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct sw_id
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct fw_id
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct cf_id
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct record_period
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct send_period
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct record_id
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct modbus1_status
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct modbus1_value_reg
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct modbus1_id_reg
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
    struct time
    {
        static bool updated;
        void set(uint32_t value, unsigned index = 0);
        uint32_t get(unsigned index = 0);
    };
};

#endif // DEVICESETTINGS_H
