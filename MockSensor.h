#pragma once
#include "Sensor.h"

class MockSensor : public Sensor
{
public:
	MockSensor();
	virtual ~MockSensor();
	virtual SensorType get_type();

protected:
	virtual bool read_sensor(double &value);

};
