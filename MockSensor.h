#pragma once
#include "Sensor.h"

class MockSensor : public Sensor
{
public:
	MockSensor(ISensorListener* listener = NULL);
	virtual ~MockSensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
