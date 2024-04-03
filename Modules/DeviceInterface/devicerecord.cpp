#include "devicerecord.h"

DeviceRecord::record_t DeviceRecord::record{};


uint16_t DeviceRecord::rcrd_id::ID;
bool* DeviceRecord::rcrd_id::updated;
void DeviceRecord::rcrd_id::set(uint64_t value, unsigned)
{
    record.id = value;
}

uint64_t DeviceRecord::rcrd_id::get(unsigned)
{
    return record.id;
}


uint16_t DeviceRecord::time::ID;
bool* DeviceRecord::time::updated;
void DeviceRecord::time::set(uint64_t value, unsigned)
{
    record.time = value;
}

uint64_t DeviceRecord::time::get(unsigned)
{
    return record.time;
}


uint16_t DeviceRecord::MODBUS1_ID::ID;
bool* DeviceRecord::MODBUS1_ID::updated;
void DeviceRecord::MODBUS1_ID::set(uint64_t value, unsigned index)
{
    record.mb1_sens[index].ID = value;
}

uint64_t DeviceRecord::MODBUS1_ID::get(unsigned index)
{
    return record.mb1_sens[index].ID;
}


uint16_t DeviceRecord::MODBUS1_value::ID;
bool* DeviceRecord::MODBUS1_value::updated;
void DeviceRecord::MODBUS1_value::set(uint64_t value, unsigned index)
{
    record.mb1_sens[index].value = value;
}

uint64_t DeviceRecord::MODBUS1_value::get(unsigned index)
{
    return record.mb1_sens[index].value;
}


uint16_t DeviceRecord::_1WIRE_ADDR::ID;
bool* DeviceRecord::_1WIRE_ADDR::updated;
void DeviceRecord::_1WIRE_ADDR::set(uint64_t value, unsigned index)
{
    record.ow_sens[index].ADDR = value;
}

uint64_t DeviceRecord::_1WIRE_ADDR::get(unsigned index)
{
    return record.ow_sens[index].ADDR;
}


uint16_t DeviceRecord::_1WIRE_value::ID;
bool* DeviceRecord::_1WIRE_value::updated;
void DeviceRecord::_1WIRE_value::set(uint64_t value, unsigned index)
{
    record.ow_sens[index].value = value;
}

uint64_t DeviceRecord::_1WIRE_value::get(unsigned index)
{
    return record.ow_sens[index].value;
}
