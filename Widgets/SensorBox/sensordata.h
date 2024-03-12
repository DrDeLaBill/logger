#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H


#include <string>


struct SensorData
{
    std::string buttonLabel;
    unsigned sensorID;
    unsigned lastID;
    unsigned number;
    unsigned idReg;
    unsigned valueReg;
    unsigned value;

    SensorData(
        const std::string& buttonLabel,
        const unsigned sensorID,
        const unsigned number,
        const unsigned idReg,
        const unsigned valueReg,
        const unsigned value
    );
    SensorData(const SensorData& other);
    SensorData& operator=(const SensorData& other);
    ~SensorData();
};

#endif
