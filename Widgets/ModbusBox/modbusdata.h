#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H


#include <string>


struct ModbusData
{
    std::string buttonLabel;
    unsigned sensorID;
    unsigned lastID;
    unsigned number;
    unsigned idReg;
    unsigned valueReg;
    int16_t  value;

    ModbusData(
        const std::string& buttonLabel,
        const unsigned sensorID,
        const unsigned number,
        const unsigned idReg,
        const unsigned valueReg,
        const int16_t  value
    );
    ModbusData(const ModbusData& other);
    ModbusData& operator=(const ModbusData& other);
    ~ModbusData();
};

#endif
