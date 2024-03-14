#include "devicerecord.h"

DeviceRecord::record_t DeviceRecord::record{};


bool* DeviceRecord::id::updated;
void DeviceRecord::id::set(uint32_t value, unsigned)
{
    record.id = value;
}

uint32_t DeviceRecord::id::get(unsigned)
{
    return record.id;
}


bool* DeviceRecord::time::updated;
void DeviceRecord::time::set(uint32_t value, unsigned)
{
    record.time = value;
}

uint32_t DeviceRecord::time::get(unsigned)
{
    return record.time;
}


bool* DeviceRecord::ID::updated;
void DeviceRecord::ID::set(uint32_t value, unsigned index)
{
    record.ID[index] = value;
}

uint32_t DeviceRecord::ID::get(unsigned index)
{
    return record.ID[index];
}


bool* DeviceRecord::value::updated;
void DeviceRecord::value::set(uint32_t value, unsigned index)
{
    record.value[index] = value;
}

uint32_t DeviceRecord::value::get(unsigned index)
{
    return record.value[index];
}
