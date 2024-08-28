#ifndef USBCSTATUS_H
#define USBCSTATUS_H


enum USBCStatus : unsigned
{
    // The task is waiting
    USBC_WAIT = 0,
    // The tasks in progress
    USBC_IN_PROGRESS,
    // The task is done
    USBC_RES_DONE,
    // USB error
    USBC_RES_ERROR,
    // Another error
    USBC_INTERNAL_ERROR
};


#endif // USBCSTATUS_H
