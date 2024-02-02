/* Copyright © 2023 Georgy E. All rights reserved. */

#ifndef _USB_HID_TABLE_H_
#define _USB_HID_TABLE_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


#define HID_VENDOR_ID        (0x0483)
#define HID_PRODUCT_ID       (0x5750)//(0xBEDA)

#define HID_INPUT_REPORT_ID  ((uint8_t)0xB1)
#define HID_OUTPUT_REPORT_ID ((uint8_t)0xB2)


typedef struct _DESCRIPTOR_UNIT {
    uint8_t* ptr;
    uint8_t  size;
} DESCRIPTOR_UNIT;


static DESCRIPTOR_UNIT USB_HID_TABLE[] = {
    {},
    {}
};


#ifdef __cplusplus
}
#endif


#endif
