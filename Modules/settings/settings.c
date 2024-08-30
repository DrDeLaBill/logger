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

bool settings_1wire_sensor_exists(uint64_t address)
{
    for (unsigned i = 0; i < __arr_len(settings_be._1wire_address); i++) {
        if (settings_be._1wire_address[i] == address) {
            return true;
        }
    }
    return false;
}


uint8_t modbus1_index_be(uint8_t index)
{
    bool found = false;
    uint8_t res = 0;
    for (uint8_t i = index; i < __arr_len(settings_be.modbus1_status); i++) {
        if (settings_be.modbus1_status[i] != SETTINGS_SENSOR_EMPTY &&
            i >= index
        ) {
            found = true;
            res = i;
            break;
        }
    }
    if (!found) {
        res = __arr_len(settings_be.modbus1_status);
    }
    return res;
}


uint8_t modbus1_index_ui(uint8_t index)
{
    bool found = false;
    uint8_t res = 0;
    for (uint8_t i = index; i < __arr_len(settings_ui.modbus1_status); i++) {
        if (settings_ui.modbus1_status[i] != SETTINGS_SENSOR_EMPTY &&
            i >= index
            ) {
            found = true;
            res = i;
            break;
        }
    }
    if (!found) {
        res = __arr_len(settings_ui.modbus1_status);
    }
    return res;
}


uint8_t _1wire_index_be(uint8_t index)
{
    bool found = false;
    uint8_t res = 0;
    for (uint8_t i = index; i < __arr_len(settings_be._1wire_address); i++) {
        if (settings_be._1wire_address[i] &&
            i >= index
        ) {
            found = true;
            res = i;
            break;
        }
    }
    if (!found) {
        res = __arr_len(settings_be._1wire_address);
    }
    return res;
}


uint8_t _1wire_index_ui(uint8_t index)
{
    bool found = false;
    uint8_t res = 0;
    for (uint8_t i = index; i < __arr_len(settings_ui._1wire_address); i++) {
        if (settings_ui._1wire_address[i] &&
            i >= index
            ) {
            found = true;
            res = i;
            break;
        }
    }
    if (!found) {
        res = __arr_len(settings_ui._1wire_address);
    }
    return res;
}
