#ifndef USBCSTATUS_H
#define USBCSTATUS_H


enum USBCStatus : unsigned
{
    // The part of the task is done
    USBC_RES_OK = 0,
    // The task is done
    USBC_RES_DONE,
    // USB error
    USBC_RES_ERROR,
    // Another error
    USBC_INTERNAL_ERROR
};


#endif // USBCSTATUS_H
