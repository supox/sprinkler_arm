#include "BatterySensor.h"

BatterySensor::BatterySensor() : Sensor()
{
}

BatterySensor::~BatterySensor()
{
}

SensorType BatterySensor::get_type()
{
	return WATER_READER;
}

bool BatterySensor::read_sensor(double &value)
{
	// TODO!
	value = 0;
	return true;
}
