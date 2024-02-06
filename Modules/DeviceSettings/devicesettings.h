#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H


#include <cstdint>


#define DV_TYPE    ((uint16_t)0x0001)
#define SW_VERSION ((uint8_t)0x04)
#define FW_VERSION ((uint8_t)0x01)
#define CF_VERSION ((uint8_t)0x01)

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
    } settings_t;

    static settings_t settings;

    bool check();

    struct dv_type           { uint16_t* operator()() { return &settings.dv_type; } };
    struct sw_id             { uint8_t*  operator()() { return &settings.sw_id; } };
    struct fw_id             { uint8_t*  operator()() { return &settings.fw_id; } };
    struct cf_id             { uint32_t* operator()() { return &settings.cf_id; } };
    struct record_period     { uint32_t* operator()() { return &settings.record_period; } };
    struct send_period       { uint32_t* operator()() { return &settings.send_period; } };
    struct record_id         { uint32_t* operator()() { return &settings.record_id; } };
    struct modbus1_status    { uint16_t* operator()() { return settings.modbus1_status; } };
    struct modbus1_value_reg { uint16_t* operator()() { return settings.modbus1_value_reg; } };
    struct modbus1_id_reg    { uint16_t* operator()() { return settings.modbus1_id_reg; } };
};

#endif // DEVICESETTINGS_H
