#include "BatterySensor.h"

BatterySensor::BatterySensor(ISensorListener* listener) : Sensor(listener)
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
