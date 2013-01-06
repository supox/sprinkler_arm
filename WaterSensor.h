#pragma once
#include "Sensor.h"

class WaterSensor : public Sensor
{
public:
	WaterSensor(ISensorListener* listener = NULL);
	virtual ~WaterSensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
