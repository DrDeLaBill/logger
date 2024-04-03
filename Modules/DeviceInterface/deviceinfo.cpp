#include "deviceinfo.h"


DeviceInfo::info_t DeviceInfo::info;


uint16_t DeviceInfo::time::ID;
bool* DeviceInfo::time::updated;
void DeviceInfo::time::set(uint64_t value, unsigned)
{
    info.time = value;
}

uint64_t DeviceInfo::time::get(unsigned)
{
    return info.time;
}


uint16_t DeviceInfo::min_id::ID;
bool* DeviceInfo::min_id::updated;
void DeviceInfo::min_id::set(uint64_t value, unsigned)
{
    info.min_id = value;
}

uint64_t DeviceInfo::min_id::get(unsigned)
{
    return info.min_id;
}


uint16_t DeviceInfo::max_id::ID;
bool* DeviceInfo::max_id::updated;
void DeviceInfo::max_id::set(uint64_t value, unsigned)
{
    info.max_id = value;
}

uint64_t DeviceInfo::max_id::get(unsigned)
{
    return info.max_id;
}


uint16_t DeviceInfo::current_id::ID;
bool* DeviceInfo::current_id::updated;
void DeviceInfo::current_id::set(uint64_t value, unsigned)
{
    info.current_id = value;
}

uint64_t DeviceInfo::current_id::get(unsigned)
{
    return info.current_id;
}


uint16_t DeviceInfo::current_mbodbus1_count::ID;
bool* DeviceInfo::current_mbodbus1_count::updated;
void DeviceInfo::current_mbodbus1_count::set(uint64_t value, unsigned)
{
    info.current_mbodbus1_count = value;
}

uint64_t DeviceInfo::current_mbodbus1_count::get(unsigned)
{
    return info.current_mbodbus1_count;
}


uint16_t DeviceInfo::current_1wire_count::ID;
bool* DeviceInfo::current_1wire_count::updated;
void DeviceInfo::current_1wire_count::set(uint64_t value, unsigned)
{
    info.current_1wire_count = value;
}

uint64_t DeviceInfo::current_1wire_count::get(unsigned)
{
    return info.current_1wire_count;
}


uint16_t DeviceInfo::need_registrate_1wire::ID;
bool* DeviceInfo::need_registrate_1wire::updated;
void DeviceInfo::need_registrate_1wire::set(uint64_t value, unsigned)
{
    info.need_registrate_1wire = value;
}

uint64_t DeviceInfo::need_registrate_1wire::get(unsigned)
{
    return info.need_registrate_1wire;
}


uint16_t DeviceInfo::record_loaded::ID;
bool* DeviceInfo::record_loaded::updated;
void DeviceInfo::record_loaded::set(uint64_t value, unsigned)
{
    info.record_loaded = value;
}

uint64_t DeviceInfo::record_loaded::get(unsigned)
{
    return info.record_loaded;
}

uint16_t DeviceInfo::modbus1_last_value::ID;
bool* DeviceInfo::modbus1_last_value::updated;
void DeviceInfo::modbus1_last_value::set(uint64_t value, unsigned index)
{
    info.modbus1_value[index] = value;
}

uint64_t DeviceInfo::modbus1_last_value::get(unsigned index)
{
    return info.modbus1_value[index];
}

