#pragma once
#include "Sensor.h"

class BatterySensor : public Sensor
{
public:
	BatterySensor(ISensorListener* listener = NULL);
	virtual ~BatterySensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
