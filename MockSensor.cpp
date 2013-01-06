#include "MockSensor.h"
#include <stdlib.h>

MockSensor::MockSensor(ISensorListener* listener) : Sensor(listener)
{
}

MockSensor::~MockSensor()
{
}

SensorType MockSensor::GetType()
{
	return MOCK;
}

bool MockSensor::ReadSensorFromHardware(double &value)
{
	// TODO!
  value = (double)(rand() % 10 + 1);
	return true;
}
