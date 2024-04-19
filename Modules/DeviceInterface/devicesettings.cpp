#include "devicesettings.h"

#include <cstring>

#include "utils.h"


DeviceSettings::settings_t DeviceSettings::settings = {};


unsigned DeviceSettings::getModbus1Index(const unsigned index)
{
    unsigned counter = __arr_len(settings_t::modbus1_status);
    for (unsigned i = index; i < __arr_len(settings_t::modbus1_status); i++) {
        if (modbus1_status{}.get(i) != SETTINGS_SENSOR_EMPTY) {
            counter = i;
            break;
        }
    }
    return counter;
}

unsigned DeviceSettings::getOnewWireIndex(const unsigned index)
{
    unsigned counter = __arr_len(settings_t::_1wire_address);
    for (unsigned i = index; i < __arr_len(settings_t::_1wire_address); i++) {
        if (_1wire_address::get(i)) {
            counter = i;
            break;
        }
    }
    return counter;
}

void DeviceSettings::clear()
{
    memset(reinterpret_cast<void*>(&settings), 0, sizeof(settings));
}

bool DeviceSettings::check()
{
    if (settings.dv_type != DEVICE_TYPE) {
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


uint16_t DeviceSettings::dv_type::ID;
bool* DeviceSettings::dv_type::updated;
void DeviceSettings::dv_type::set(uint64_t value, unsigned)
{
    settings.dv_type = value;
}

uint64_t DeviceSettings::dv_type::get(unsigned)
{
    return settings.dv_type;
}

uint16_t DeviceSettings::sw_id::ID;
bool* DeviceSettings::sw_id::updated;
void DeviceSettings::sw_id::set(uint64_t value, unsigned)
{
    settings.sw_id = value;
}

uint64_t DeviceSettings::sw_id::get(unsigned)
{
    return settings.sw_id;
}

uint16_t DeviceSettings::fw_id::ID;
bool* DeviceSettings::fw_id::updated;
void DeviceSettings::fw_id::set(uint64_t value, unsigned)
{
    settings.fw_id = value;
}

uint64_t DeviceSettings::fw_id::get(unsigned)
{
    return settings.fw_id;
}

uint16_t DeviceSettings::cf_id::ID;
bool* DeviceSettings::cf_id::updated;
void DeviceSettings::cf_id::set(uint64_t value, unsigned)
{
    settings.cf_id = value;
}

uint64_t DeviceSettings::cf_id::get(unsigned)
{
    return settings.cf_id;
}

uint16_t DeviceSettings::record_period::ID;
bool* DeviceSettings::record_period::updated;
void DeviceSettings::record_period::set(uint64_t value, unsigned)
{
    settings.record_period = value;
}

uint64_t DeviceSettings::record_period::get(unsigned)
{
    return settings.record_period;
}

uint16_t DeviceSettings::send_period::ID;
bool* DeviceSettings::send_period::updated;
void DeviceSettings::send_period::set(uint64_t value, unsigned)
{
    settings.send_period = value;
}

uint64_t DeviceSettings::send_period::get(unsigned)
{
    return settings.send_period;
}

uint16_t DeviceSettings::record_id::ID;
bool* DeviceSettings::record_id::updated;
void DeviceSettings::record_id::set(uint64_t value, unsigned)
{
    settings.record_id = value;
}

uint64_t DeviceSettings::record_id::get(unsigned)
{
    return settings.record_id;
}

uint16_t DeviceSettings::modbus1_status::ID;
bool* DeviceSettings::modbus1_status::updated;
void DeviceSettings::modbus1_status::set(uint64_t value, unsigned index)
{
    settings.modbus1_status[index] = value;
}

uint64_t DeviceSettings::modbus1_status::get(unsigned index)
{
    return settings.modbus1_status[index];
}

uint16_t DeviceSettings::modbus1_value_reg::ID;
bool* DeviceSettings::modbus1_value_reg::updated;
void DeviceSettings::modbus1_value_reg::set(uint64_t value, unsigned index)
{
    settings.modbus1_value_reg[index] = value;
}

uint64_t DeviceSettings::modbus1_value_reg::get(unsigned index)
{
    return settings.modbus1_value_reg[index];
}

uint16_t DeviceSettings::modbus1_id_reg::ID;
bool* DeviceSettings::modbus1_id_reg::updated;
void DeviceSettings::modbus1_id_reg::set(uint64_t value, unsigned index)
{
    settings.modbus1_id_reg[index] = value;
}

uint64_t DeviceSettings::modbus1_id_reg::get(unsigned index)
{
    return settings.modbus1_id_reg[index];
}

uint16_t DeviceSettings::_1wire_address::ID;
bool* DeviceSettings::_1wire_address::updated;
void DeviceSettings::_1wire_address::set(uint64_t value, unsigned index)
{
    settings._1wire_address[index] = value;
}

uint64_t DeviceSettings::_1wire_address::get(unsigned index)
{
    return settings._1wire_address[index];
}


uint16_t DeviceSettings::mb1_last_id::ID;
bool* DeviceSettings::mb1_last_id::updated;
void DeviceSettings::mb1_last_id::set(uint8_t value, unsigned)
{
    settings.mb1_last_id = value;
}
uint8_t DeviceSettings::mb1_last_id::get(unsigned)
{
    return settings.mb1_last_id;
}


uint16_t DeviceSettings::mb1_new_id::ID;
bool* DeviceSettings::mb1_new_id::updated;
void DeviceSettings::mb1_new_id::set(uint8_t value, unsigned)
{
    settings.mb1_new_id = value;
}
uint8_t DeviceSettings::mb1_new_id::get(unsigned)
{
    return settings.mb1_new_id;
}


uint16_t DeviceSettings::need_mb1_id_update::ID;
bool* DeviceSettings::need_mb1_id_update::updated;
void DeviceSettings::need_mb1_id_update::set(uint8_t value, unsigned)
{
    settings.need_mb1_id_update = value;
}
uint8_t DeviceSettings::need_mb1_id_update::get(unsigned)
{
    return settings.need_mb1_id_update;
}
