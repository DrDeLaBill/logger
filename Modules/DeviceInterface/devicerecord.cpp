#include "devicerecord.h"

DeviceRecord::record_t DeviceRecord::record{};


uint16_t DeviceRecord::rcrd_id::ID;
bool* DeviceRecord::rcrd_id::updated;
void DeviceRecord::rcrd_id::set(uint32_t value, unsigned)
{
    record.id = value;
}

uint32_t DeviceRecord::rcrd_id::get(unsigned)
{
    return record.id;
}


uint16_t DeviceRecord::time::ID;
bool* DeviceRecord::time::updated;
void DeviceRecord::time::set(uint32_t value, unsigned)
{
    record.time = value;
}

uint32_t DeviceRecord::time::get(unsigned)
{
    return record.time;
}


uint16_t DeviceRecord::snsr_id::ID;
bool* DeviceRecord::snsr_id::updated;
void DeviceRecord::snsr_id::set(uint32_t value, unsigned index)
{
    record.ID[index] = value;
}

uint32_t DeviceRecord::snsr_id::get(unsigned index)
{
    return record.ID[index];
}


uint16_t DeviceRecord::value::ID;
bool* DeviceRecord::value::updated;
void DeviceRecord::value::set(uint32_t value, unsigned index)
{
    record.value[index] = value;
}

uint32_t DeviceRecord::value::get(unsigned index)
{
    return record.value[index];
}
