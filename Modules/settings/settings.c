/* Copyright © 2023 Georgy E. All rights reserved. */

#include "settings.h"

#include "gutils.h"


settings_t settings_be = {0};
settings_t settings_ui = {0};


bool settings_check(settings_t* other)
{
    if (other->dv_type != DEVICE_TYPE) {
        return false;
    }
    if (other->sw_id != SW_VERSION) {
        return false;
    }
    if (other->fw_id != FW_VERSION) {
        return false;
    }

    return true;
}

uint8_t modbus1_index(uint8_t index)
{
    uint8_t counter = __arr_len(settings_be.modbus1_status);
    for (uint8_t i = index; i < __arr_len(settings_be.modbus1_status); i++) {
        if (settings_be.modbus1_status[i] != SETTINGS_SENSOR_EMPTY) {
            counter++;
        }
        if (counter == index) {
            break;
        }
    }
    return counter;
}

uint8_t _1wire_index(uint8_t index)
{
    uint8_t counter = __arr_len(settings_be._1wire_address);
    for (uint8_t i = index; i < __arr_len(settings_be._1wire_address); i++) {
        if (settings_be._1wire_address[i]) {
            counter++;
        }
        if (counter == index) {
            break;
        }
    }
    return counter;
}
