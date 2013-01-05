#pragma once
#include "Sensor.h"

class WaterSensor : public Sensor
{
public:
	WaterSensor();
	virtual ~WaterSensor();
	virtual SensorType get_type();

protected:
	virtual bool read_sensor(double &value);

};
