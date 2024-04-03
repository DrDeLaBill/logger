#include "modbusdata.h"


ModbusData::ModbusData(
    const std::string& buttonLabel,
    const unsigned sensorID,
    const unsigned number,
    const unsigned idReg,
    const unsigned valueReg,
    const unsigned value
    ):  buttonLabel(buttonLabel), sensorID(sensorID), lastID(sensorID), number(number),
    idReg(idReg), valueReg(valueReg), value(value) {}

ModbusData::ModbusData(const ModbusData &other):
    ModbusData(other.buttonLabel, other.sensorID, other.number, other.idReg, other.valueReg, other.value)
{}

ModbusData& ModbusData::operator=(const ModbusData& other)
{
    buttonLabel = other.buttonLabel;
    sensorID = other.sensorID;
    lastID = other.lastID;
    number = other.number;
    idReg = other.idReg;
    valueReg = other.valueReg;
    value = other.value;
    return *this;
}

ModbusData::~ModbusData() {}
