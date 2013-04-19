#pragma once
#include "Sensor.h"

class MockSensor : public Sensor
{
public:
	MockSensor(const int id, const int port_index, ISensorListener* listener = NULL);
	virtual ~MockSensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
