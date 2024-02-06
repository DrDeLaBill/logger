#include "usbhandler.h"

#include "Modules/HIDService/hidservice.h"
#include "Modules/HIDTable/HIDController.h"


bool USBHandlerEqual::operator()(const QString& lhs, const QString& rhs) const
{
    return lhs.toStdString() == rhs.toStdString();
}

std::size_t USBHandlerHash::operator()(const QString& key) const
{
    return std::hash<std::string>{}(key.toStdString());
}

void USBSearchHandler::operate() const
{
    HIDService::init();

    while (!HIDService::isDeviceConnected(HID_VENDOR_ID, HID_PRODUCT_ID));

    HIDService::deinit();
}

void USBReportHandler::operate() const
{
    HIDService::init();

    HIDService::loadReport(HID_VENDOR_ID, HID_PRODUCT_ID);

    HIDService::deinit();
}

void USBCGetharacteristicHandler::operate() const
{
    HIDService::init();

    HIDService::loadReport(HID_VENDOR_ID, HID_PRODUCT_ID);

    HIDService::deinit();
}
