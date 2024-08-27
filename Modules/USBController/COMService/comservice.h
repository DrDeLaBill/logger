#ifndef COMSERVICE_H
#define COMSERVICE_H


#include <memory>
#include <cstdint>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "greport.h"


#define COM_SERVICE_BEDUG (false)


class COMService
{
private:
    static constexpr char TAG[] = "COMs";

    static std::string m_portName;

    static std::unique_ptr<QSerialPort> port;

public:
    static void begin(const std::string& portName);

    static void sendReport(const pack_t& report);

    static void deinit();

    static bool available();

private:
    // TODO: https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
    static void handleException();


};

#endif // COMSERVICE_H
