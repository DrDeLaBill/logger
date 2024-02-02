#include "hidservice.h"

#include <libusb.h>

#include "app_exception.h"


void HIDService::init()
{
    if (libusb_init(NULL) != 0) {
        throw new exceptions::UsbInitException("Unable to initialize USB");
    }
}

void HIDService::deinit()
{
    libusb_exit(NULL);
}

bool HIDService::isDeviceConnected(uint16_t vendorId, uint16_t productId)
{
    libusb_device* dev = nullptr;
    return findDevice(dev, vendorId, productId);
}

void HIDService::showReport(uint16_t vendorId, uint16_t productId)
{
    libusb_device* dev = nullptr;
    if (!findDevice(dev, vendorId, productId)) {
        throw new exceptions::UsbNotFoundException("Device not found");
    }
}

bool HIDService::findDevice(libusb_device *dev, uint16_t vendorId, uint16_t productId)
{
    libusb_device **devs;

    if (libusb_get_device_list(NULL, &devs) <=   0) {
        return false;
    }

    int i = 0;
    bool result = false;
    while ((dev = devs[i++]) != NULL) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(dev, &desc) != 0) {
            continue;
        }
        if (desc.idVendor == vendorId && desc.idProduct == productId) {
            result = true;
            break;
        }
    }

    libusb_free_device_list(devs, 1);

    return result;
}
