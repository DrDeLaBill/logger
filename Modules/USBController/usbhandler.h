#ifndef USBHANDLER_H
#define USBHANDLER_H


struct USBHandlerBase {};

struct USBSearchHandler: public USBHandlerBase {};
struct USBReportHandler: public USBHandlerBase {};


#endif // USBHANDLER_H
