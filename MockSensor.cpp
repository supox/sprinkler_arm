#include "MockSensor.h"
#include <stdlib.h>

MockSensor::MockSensor() : Sensor()
{
}

MockSensor::~MockSensor()
{
}

SensorType MockSensor::get_type()
{
	return WATER_READER;
}

bool MockSensor::read_sensor(double &value)
{
	// TODO!
  value = (double)(rand() % 10 + 1);
	return true;
}
