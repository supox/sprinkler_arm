#include "MockSensor.h"
#include <stdlib.h>

MockSensor::MockSensor(const int id, const int port_index, const double sensor_value, ISensorListener* listener) : Sensor(id, port_index, sensor_value, listener)
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
