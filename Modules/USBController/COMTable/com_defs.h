/* Copyright © 2024 Georgy E. All rights reserved. */

#ifndef _COM_DEFS_H_
#define _COM_DEFS_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>

#include "utils.h"


#define COM_PRODUCT_ID         (0xBEDA)

#define COM_INTERFACE          (0)
#define COM_CONFIGURATION      (0)

#define COM_SEND_FLAG          ((uint8_t)0x01)
#define COM_GETTER_ID          ((uint8_t)0x00)

#define COM_FIRST_KEY          ((uint16_t)1)

#define COM_DELAY_MS           (25)

#define COM_TABLE_BEDUG        (true)


static const char COM_TAG[] = "COM";


#ifdef __MINGW32__
#   pragma pack(push, 1)
#else
TYPE_PACK(
#endif
typedef struct
#ifndef __MINGW32__
,
#endif
_report_pack_t {
    uint8_t  flag;
    uint16_t characteristic_id;
    uint8_t  index;
    uint8_t  data[sizeof(uint32_t)];
    uint16_t crc;
} report_pack_t;
#ifdef __MINGW32__
#   pragma pack(pop)
#else
);
#endif


uint16_t com_get_crc(const report_pack_t* report);

#ifdef USE_HAL_DRIVER
bool com_get_report(report_pack_t* report);
bool com_send_report(report_pack_t* report);
#endif

void com_report_show(const report_pack_t* report);

void com_report_set_data(report_pack_t* report, const uint8_t* src_data, const unsigned size);


#ifdef __cplusplus
}
#endif


#endif /* HIDTABLE_COM_DEFS_H_ */
