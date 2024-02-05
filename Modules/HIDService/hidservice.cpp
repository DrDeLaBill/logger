#include "hidservice.h"

#include <cstdio>
#include <cstdint>
#include <memory>

#include <libusb.h>

#include "Modules/Exceptions/app_exception.h"


void HIDService::init()
{
    if (libusb_init(NULL) != 0) {
        throw new exceptions::UsbInitException();
    }
//    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);
}

void HIDService::deinit()
{
    libusb_exit(NULL);
}

bool HIDService::isDeviceConnected(uint16_t vendorId, uint16_t productId)
{
    return checkDevice(vendorId, productId);
}

void HIDService::showReport(uint16_t vendorId, uint16_t productId)
{
    libusb_device **devs = NULL;
    if (libusb_get_device_list(NULL, &devs) <= 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbNotFoundException();
    }

    libusb_device* dev = findDevice(devs, vendorId, productId);
    if (dev == NULL) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbNotFoundException();
    }

    libusb_device_handle *handle = NULL;
    if (libusb_open(dev, &handle) != 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbNotFoundException();
    }

    struct libusb_device_descriptor dev_desc;
    if (libusb_get_device_descriptor(dev, &dev_desc) != 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbReportException();
    }

    std::unique_ptr<uint8_t[]> report = std::make_unique<uint8_t[]>(dev_desc.bLength);

    if (libusb_claim_interface(handle, 0) != 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbReportException();
    }

    int length = 0;
    if (libusb_interrupt_transfer(handle, 0x81, report.get(), dev_desc.bLength, &length, 1000) != 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbReportException();
    }
    if (length == 0) {
        libusb_free_device_list(devs, 1);
        throw new exceptions::UsbReportException();
    }

    libusb_free_device_list(devs, 1);

    for (int i = 0; i < length; i++) {
        printf("%02X ", report[i]);
    }
    printf("\n");
}

bool HIDService::checkDevice(uint16_t vendorId, uint16_t productId)
{
    libusb_device **devs;

    if (libusb_get_device_list(NULL, &devs) <=   0) {
        return false;
    }

    bool result = true;
    libusb_device *dev = findDevice(devs, vendorId, productId);
    if (dev == NULL) {
        result = false;
    }

    libusb_free_device_list(devs, 1);

    return result;
}

libusb_device* HIDService::findDevice(libusb_device **devs, uint16_t vendorId, uint16_t productId)
{
    int i = 0;
    libusb_device* dev = NULL;
    while ((dev = devs[i++]) != NULL) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(dev, &desc) != 0) {
            continue;
        }
        if (desc.idVendor == vendorId && desc.idProduct == productId) {
            break;
        }
    }
    return dev;
}
