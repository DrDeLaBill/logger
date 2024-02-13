#include "hidservice.h"

#include <cstdio>
#include <cstdint>
#include <memory>
#include <iostream>

#include <libusb.h>

#include "log.h"
#include "variables.h"

#include "hid_defs.h"
#include "usbdreport.h"
#include "usbhreport.h"
#include "app_exception.h"


libusb_device_handle* HIDService::handle = NULL;
libusb_device** HIDService:: devs = NULL;

void HIDService::init()
{
    if (libusb_init(NULL) != 0) {
        throw exceptions::UsbInitException();
    }
}

void HIDService::deinit()
{
    libusb_exit(NULL);
}

bool HIDService::isDeviceConnected(uint16_t vendorId, uint16_t productId)
{
    return checkDevice(vendorId, productId);
}

void HIDService::sendReport(uint16_t vendorId, uint16_t productId, const report_pack_t& request)
{
    struct libusb_device_descriptor dev_desc;

    try {
        initSession(&dev_desc, vendorId, productId);
    } catch (...) {
        closeSession();
        throw;
    }

    USBHReport::createReport(request);

    int resLength = 0;
    try {
        int res = libusb_interrupt_transfer(
            handle,
            HID_OUTPUT_ENDPOINT,
            reinterpret_cast<uint8_t*>(&USBHReport::getReport()),
            dev_desc.bLength,
            &resLength,
            HID_DELAY_MS
        );
        if (res < 0) {
            // TODO: timeout for timeouts (if device has another version)
            throw exceptions::UsbTimeoutException();
        }
        if (resLength != sizeof(request)) {
            throw exceptions::UsbReportException();
        }

        report_pack_t response = {};
        res = libusb_interrupt_transfer(
            handle,
            HID_INPUT_ENDPOINT,
            reinterpret_cast<uint8_t*>(&response),
            dev_desc.bLength,
            &resLength,
            HID_DELAY_MS
        );
        if (res < 0) {
            // TODO: timeout for timeouts (if device has another version)
            throw exceptions::UsbTimeoutException();
        }

        try {
            USBDReport::setReport(response);
        } catch (...) {
            throw;
        }

        if (resLength == 0) {
            throw exceptions::UsbReportException();
        }

    } catch (...) {
        closeSession();
        throw;
    }

    closeSession();

#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "HOST REPORT:");
    USBHReport::getReport().show();
    printTagLog(TAG, "DEVICE REPORT:");
    USBDReport::getReport().show();
    printPretty("deserializated: %u\n", utl::deserialize<uint32_t>(USBDReport::getReport().data)[0]);
#endif
}

void HIDService::loadReport(uint16_t vendorId, uint16_t productId)
{
    struct libusb_device_descriptor dev_desc;

    try {
        initSession(&dev_desc, vendorId, productId);
    } catch (...) {
        closeSession();
        throw;
    }

    int resLength = 0;
    report_pack_t report = {};
    try {
        int res = libusb_interrupt_transfer(
            handle,
            HID_INPUT_ENDPOINT,
            reinterpret_cast<uint8_t*>(&report),
            dev_desc.bLength,
            &resLength,
            HID_DELAY_MS
        );
        if (res < 0) {
            // TODO: timeout for timeouts (if device has another version)
            throw exceptions::UsbTimeoutException();
        }

        try {
            USBDReport::setReport(report);
        } catch (...) {
            throw;
        }

        if (resLength == 0) {
            throw exceptions::UsbReportException();
        }
    } catch (...) {
        closeSession();
        throw;
    }

    closeSession();

#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "DEVICE REPORT:");
    USBDReport::getReport().show();
#endif
}

void HIDService::initSession(
    libusb_device_descriptor* dev_desc,
    uint16_t                  vendorId,
    uint16_t                  productId
) {
    if (libusb_get_device_list(NULL, &devs) <= 0) {
        throw exceptions::UsbNotFoundException();
    }

    libusb_device* dev = findDevice(devs, vendorId, productId);
    if (dev == NULL) {
        throw exceptions::UsbNotFoundException();
    }

    handle = NULL;
    if (libusb_open(dev, &handle) != 0) {
        throw exceptions::UsbNotFoundException();
    }

    int res = 0;
    if (libusb_kernel_driver_active(handle, HID_INTERFACE)) {
        res = libusb_detach_kernel_driver(handle, HID_INTERFACE);
    }
//    if (res < 0) {
//        throw exceptions::UsbAccessException();
//    }

    // TODO: get descriptor from findDevice func
    if (libusb_get_device_descriptor(dev, dev_desc) != 0) {
        throw exceptions::UsbReportException();
    }

    if (libusb_claim_interface(handle, HID_INTERFACE) != 0) {
        throw exceptions::UsbReportException();
    }
}

void HIDService::closeSession() {
    if (handle) {
        libusb_attach_kernel_driver(handle, HID_INTERFACE);
    }

    if (devs) {
        libusb_free_device_list(devs, 1);
    }
}

bool HIDService::checkDevice(uint16_t vendorId, uint16_t productId)
{
    libusb_device **devs;

    if (libusb_get_device_list(NULL, &devs) <= 0) {
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
