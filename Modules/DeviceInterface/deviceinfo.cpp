#include "deviceinfo.h"


DeviceInfo::info_t DeviceInfo::info;


bool DeviceInfo::time::updated = false;
void DeviceInfo::time::set(uint32_t value, unsigned)
{
    info.time = value;
}

uint32_t DeviceInfo::time::get(unsigned)
{
    return info.time;
}
