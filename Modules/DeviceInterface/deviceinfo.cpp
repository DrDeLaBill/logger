#include "deviceinfo.h"


DeviceInfo::info_t DeviceInfo::info;


uint16_t DeviceInfo::time::ID;
bool* DeviceInfo::time::updated;
void DeviceInfo::time::set(uint32_t value, unsigned)
{
    info.time = value;
}

uint32_t DeviceInfo::time::get(unsigned)
{
    return info.time;
}


uint16_t DeviceInfo::min_id::ID;
bool* DeviceInfo::min_id::updated;
void DeviceInfo::min_id::set(uint32_t value, unsigned)
{
    info.min_id = value;
}

uint32_t DeviceInfo::min_id::get(unsigned)
{
    return info.min_id;
}


uint16_t DeviceInfo::max_id::ID;
bool* DeviceInfo::max_id::updated;
void DeviceInfo::max_id::set(uint32_t value, unsigned)
{
    info.max_id = value;
}

uint32_t DeviceInfo::max_id::get(unsigned)
{
    return info.max_id;
}


uint16_t DeviceInfo::current_id::ID;
bool* DeviceInfo::current_id::updated;
void DeviceInfo::current_id::set(uint32_t value, unsigned)
{
    info.current_id = value;
}

uint32_t DeviceInfo::current_id::get(unsigned)
{
    return info.current_id;
}


uint16_t DeviceInfo::current_count::ID;
bool* DeviceInfo::current_count::updated;
void DeviceInfo::current_count::set(uint32_t value, unsigned)
{
    info.current_count = value;
}

uint32_t DeviceInfo::current_count::get(unsigned)
{
    return info.current_count;
}


uint16_t DeviceInfo::record_loaded::ID;
bool* DeviceInfo::record_loaded::updated;
void DeviceInfo::record_loaded::set(uint32_t value, unsigned)
{
    info.record_loaded = value;
}

uint32_t DeviceInfo::record_loaded::get(unsigned)
{
    return info.record_loaded;
}

uint16_t DeviceInfo::modbus1_value::ID;
bool* DeviceInfo::modbus1_value::updated;
void DeviceInfo::modbus1_value::set(uint32_t value, unsigned index)
{
    info.modbus1_value[index] = value;
}

uint32_t DeviceInfo::modbus1_value::get(unsigned index)
{
    return info.modbus1_value[index];
}

