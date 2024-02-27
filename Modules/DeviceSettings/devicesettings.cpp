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


void DeviceSettings::dv_type::set(uint64_t value, unsigned)
{
    settings.dv_type = value;
}

uint64_t DeviceSettings::dv_type::get(unsigned)
{
    return settings.dv_type;
}

void DeviceSettings::sw_id::set(uint64_t value, unsigned)
{
    settings.sw_id = value;
}

uint64_t DeviceSettings::sw_id::get(unsigned)
{
    return settings.sw_id;
}

void DeviceSettings::fw_id::set(uint64_t value, unsigned)
{
    settings.fw_id = value;
}

uint64_t DeviceSettings::fw_id::get(unsigned)
{
    return settings.fw_id;
}

void DeviceSettings::cf_id::set(uint64_t value, unsigned)
{
    settings.cf_id = value;
}

uint64_t DeviceSettings::cf_id::get(unsigned)
{
    return settings.cf_id;
}

void DeviceSettings::record_period::set(uint64_t value, unsigned)
{
    settings.record_period = value;
}

uint64_t DeviceSettings::record_period::get(unsigned)
{
    return settings.record_period;
}

void DeviceSettings::send_period::set(uint64_t value, unsigned)
{
    settings.send_period = value;
}

uint64_t DeviceSettings::send_period::get(unsigned)
{
    return settings.send_period;
}

void DeviceSettings::record_id::set(uint64_t value, unsigned)
{
    settings.record_id = value;
}

uint64_t DeviceSettings::record_id::get(unsigned)
{
    return settings.record_id;
}

void DeviceSettings::modbus1_status::set(uint64_t value, unsigned index)
{
    settings.modbus1_status[index] = value;
}

uint64_t DeviceSettings::modbus1_status::get(unsigned index)
{
    return settings.modbus1_status[index];
}

void DeviceSettings::modbus1_value_reg::set(uint64_t value, unsigned index)
{
    settings.modbus1_value_reg[index] = value;
}

uint64_t DeviceSettings::modbus1_value_reg::get(unsigned index)
{
    return settings.modbus1_value_reg[index];
}

void DeviceSettings::modbus1_id_reg::set(uint64_t value, unsigned index)
{
    settings.modbus1_id_reg[index] = value;
}

uint64_t DeviceSettings::modbus1_id_reg::get(unsigned index)
{
    return settings.modbus1_id_reg[index];
}
