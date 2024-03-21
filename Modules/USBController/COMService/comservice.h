#ifndef COMSERVICE_H
#define COMSERVICE_H


#include <memory>
#include <cstdint>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "com_defs.h"


#define COM_SERVICE_BEDUG (false)


class COMService
{
private:
    static constexpr char TAG[] = "COMs";

    static std::unique_ptr<QSerialPort> port;

public:
    static void init(QString portName);
    static void deinit();

    static bool isDeviceConnected(uint16_t vendorId, uint16_t productId);

    static void sendReport(const report_pack_t& report);
    static unsigned getReportSize(uint16_t vendorId, uint16_t productId);
    static std::unique_ptr<uint8_t> getReport(uint16_t vendorId, uint16_t productId);

private:
    // TODO: https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
    static void handleException();


};

#endif // COMSERVICE_H
