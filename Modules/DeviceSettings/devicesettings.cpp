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
