#ifndef DEVICEDEFS_H
#define DEVICEDEFS_H


#include <cstdint>


/*
 * Device types:
 * 0x0001 - Dispenser
 * 0x0002 - Gas station
 * 0x0003 - Logger
 * 0x0004 - B.O.B.A.
 */
#define DEVICE_TYPE        ((uint16_t)0x0003)
#define SW_VERSION         ((uint8_t)0x05)
#define FW_VERSION         ((uint8_t)0x01)

#define MODBUS_SENS_COUNT  ((uint8_t)127)


#endif // DEVICEDEFS_H
