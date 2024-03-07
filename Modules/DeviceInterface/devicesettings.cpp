#include "devicesettings.h"


DeviceSettings::settings_t DeviceSettings::settings = {};


bool DeviceSettings::check()
{
    if (settings.dv_type != DV_TYPE) {
        return false;
    }
    if (settings.sw_id != SW_VERSION) {
        return false;
    }
    if (settings.fw_id != FW_VERSION) {
        return false;
    }
    return true;
}


bool DeviceSettings::dv_type::updated = false;
void DeviceSettings::dv_type::set(uint32_t value, unsigned)
{
    settings.dv_type = value;
}

uint32_t DeviceSettings::dv_type::get(unsigned)
{
    return settings.dv_type;
}

bool DeviceSettings::sw_id::updated = false;
void DeviceSettings::sw_id::set(uint32_t value, unsigned)
{
    settings.sw_id = value;
}

uint32_t DeviceSettings::sw_id::get(unsigned)
{
    return settings.sw_id;
}

bool DeviceSettings::fw_id::updated = false;
void DeviceSettings::fw_id::set(uint32_t value, unsigned)
{
    settings.fw_id = value;
}

uint32_t DeviceSettings::fw_id::get(unsigned)
{
    return settings.fw_id;
}

bool DeviceSettings::cf_id::updated = false;
void DeviceSettings::cf_id::set(uint32_t value, unsigned)
{
    settings.cf_id = value;
}

uint32_t DeviceSettings::cf_id::get(unsigned)
{
    return settings.cf_id;
}

bool DeviceSettings::record_period::updated = false;
void DeviceSettings::record_period::set(uint32_t value, unsigned)
{
    settings.record_period = value;
}

uint32_t DeviceSettings::record_period::get(unsigned)
{
    return settings.record_period;
}

bool DeviceSettings::send_period::updated = false;
void DeviceSettings::send_period::set(uint32_t value, unsigned)
{
    settings.send_period = value;
}

uint32_t DeviceSettings::send_period::get(unsigned)
{
    return settings.send_period;
}

bool DeviceSettings::record_id::updated = false;
void DeviceSettings::record_id::set(uint32_t value, unsigned)
{
    settings.record_id = value;
}

uint32_t DeviceSettings::record_id::get(unsigned)
{
    return settings.record_id;
}

bool DeviceSettings::modbus1_status::updated = false;
void DeviceSettings::modbus1_status::set(uint32_t value, unsigned index)
{
    settings.modbus1_status[index] = value;
}

uint32_t DeviceSettings::modbus1_status::get(unsigned index)
{
    return settings.modbus1_status[index];
}

bool DeviceSettings::modbus1_value_reg::updated = false;
void DeviceSettings::modbus1_value_reg::set(uint32_t value, unsigned index)
{
    settings.modbus1_value_reg[index] = value;
}

uint32_t DeviceSettings::modbus1_value_reg::get(unsigned index)
{
    return settings.modbus1_value_reg[index];
}

bool DeviceSettings::modbus1_id_reg::updated = false;
void DeviceSettings::modbus1_id_reg::set(uint32_t value, unsigned index)
{
    settings.modbus1_id_reg[index] = value;
}

uint32_t DeviceSettings::modbus1_id_reg::get(unsigned index)
{
    return settings.modbus1_id_reg[index];
}
