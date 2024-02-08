#ifndef HIDSERVICE_H
#define HIDSERVICE_H


#include <memory>
#include <cstdint>

#include <libusb.h>

#include "hid_defs.h"


class HIDService
{
private:
    static constexpr char TAG[] = "HIDS";

    std::unique_ptr<uint8_t> report;

public:
    static void init();
    static void deinit();

    static bool isDeviceConnected(uint16_t vendorId, uint16_t productId);

    static void loadReport(uint16_t vendorId, uint16_t productId);
    static void sendReport(uint16_t vendorId, uint16_t productId, const report_pack_t& report);
    static unsigned getReportSize(uint16_t vendorId, uint16_t productId);
    static std::unique_ptr<uint8_t> getReport(uint16_t vendorId, uint16_t productId);

private:
    static bool checkDevice(uint16_t vendorId, uint16_t productId);
    static libusb_device* findDevice(libusb_device **devs, uint16_t vendorId, uint16_t productId);

    static void initSession(
        libusb_device_handle *handle,
        libusb_device **devs,
        libusb_device_descriptor* dev_desc,
        uint16_t vendorId,
        uint16_t productId
    );
    static void closeSession(
        libusb_device_handle *handle,
        libusb_device **devs
    );

    // TODO: https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
    static void handleException();
};

#endif // HIDSERVICE_H
