#include "MockSensor.h"
#include <stdlib.h>

MockSensor::MockSensor(const int id, const int port_index, ISensorListener* listener) : Sensor(id, port_index, listener)
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
