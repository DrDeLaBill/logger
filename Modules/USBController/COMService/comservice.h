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

    std::unique_ptr<QSerialPort> port;

public:
    ~COMService();

    void init(const std::string& portName);

    void sendReport(const report_pack_t& report);

    void deinit();

private:
    // TODO: https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
    static void handleException();


};

#endif // COMSERVICE_H
