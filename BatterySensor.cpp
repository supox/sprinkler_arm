#include "BatterySensor.h"

BatterySensor::BatterySensor(const int id, const int port_index, ISensorListener* listener) : Sensor(id, port_index, listener)
{
}

BatterySensor::~BatterySensor()
{
}

SensorType BatterySensor::GetType()
{
	return BATTERY;
}

bool BatterySensor::ReadSensorFromHardware(double &value)
{
	// TODO!
	value = 0;
	return true;
}
