#ifndef HIDSERVICE_H
#define HIDSERVICE_H


#include <memory>
#include <cstdint>

#include <libusb.h>


class HIDService
{
private:
    std::unique_ptr<uint8_t> report;

public:
    static void init();
    static void deinit();

    static bool isDeviceConnected(uint16_t vendorId, uint16_t productId);

    static void showReport(uint16_t vendorId, uint16_t productId);
    static unsigned getReportSize(uint16_t vendorId, uint16_t productId);
    static std::unique_ptr<uint8_t> getReport(uint16_t vendorId, uint16_t productId);

private:
    static bool findDevice(libusb_device* desc, uint16_t vendorId, uint16_t productId);
};

#endif // HIDSERVICE_H
