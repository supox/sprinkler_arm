#include "BatterySensor.h"

BatterySensor::BatterySensor(const int id, const int port_index, const double sensor_value, ISensorListener* listener) : Sensor(id, port_index, sensor_value, listener)
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
