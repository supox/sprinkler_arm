#pragma once
#include "Sensor.h"

class MockSensor : public Sensor
{
public:
	MockSensor(const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
	virtual ~MockSensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
