#include "usbhandler.h"

#include "Modules/HIDService/hidservice.h"
#include "Modules/USB_HID_Table/USB_HID_Table.h"


bool USBHandlerEqual::operator()(const QString& lhs, const QString& rhs) const
{
    return lhs.toStdString() == rhs.toStdString();
}

std::size_t USBHandlerHash::operator()(const QString& key) const
{
    return std::hash<std::string>{}(key.toStdString());
}

void USBSearchHandler::operate()
{
    HIDService::init();

    while (!HIDService::isDeviceConnected(HID_VENDOR_ID, HID_PRODUCT_ID));

    HIDService::deinit();
}

void USBReportHandler::operate()
{
    HIDService::init();

    HIDService::deinit();
}
