#ifndef HIDSERVICE_H
#define HIDSERVICE_H


#include <memory>
#include <cstdint>

#include <libusb.h>

#include "hid_defs.h"


#define HID_SERVICE_BEDUG (false)


class HIDService
{
private:
    static constexpr char TAG[] = "HIDS";

    std::unique_ptr<uint8_t> report;

public:
    static void init(uint16_t vendorId, uint16_t productId);
    static void deinit();

    static bool isDeviceConnected(uint16_t vendorId, uint16_t productId);

    static void loadReport();
    static void sendReport(const report_pack_t& report);
    static unsigned getReportSize(uint16_t vendorId, uint16_t productId);
    static std::unique_ptr<uint8_t> getReport(uint16_t vendorId, uint16_t productId);

private:
    static libusb_device_handle* handle;
    static libusb_device** devs;
    static struct libusb_device_descriptor dev_desc;

    static bool checkDevice(uint16_t vendorId, uint16_t productId);
    static libusb_device* findDevice(libusb_device **devs, uint16_t vendorId, uint16_t productId);

    static void initSession(
        libusb_device_descriptor* dev_desc,
        uint16_t                  vendorId,
        uint16_t                  productId
    );
    static void closeSession();

    // TODO: https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
    static void handleException();
};

#endif // HIDSERVICE_H
