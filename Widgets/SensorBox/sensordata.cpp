#include "sensordata.h"


SensorData::SensorData(
    const std::string& buttonLabel,
    const unsigned sensorID,
    const unsigned number,
    const unsigned idReg,
    const unsigned valueReg,
    const unsigned value
    ):  buttonLabel(buttonLabel), sensorID(sensorID), lastID(sensorID), number(number),
    idReg(idReg), valueReg(valueReg), value(value) {}

SensorData::SensorData(const SensorData &other):
    SensorData(other.buttonLabel, other.sensorID, other.number, other.idReg, other.valueReg, other.value)
{}

SensorData& SensorData::operator=(const SensorData& other)
{
    buttonLabel = other.buttonLabel;
    sensorID = other.sensorID;
    number = other.number;
    idReg = other.idReg;
    valueReg = other.valueReg;
    value = other.value;
    return *this;
}

SensorData::~SensorData() {}
