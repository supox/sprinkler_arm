#pragma once
#include "Sensor.h"

class BatterySensor : public Sensor
{
public:
	BatterySensor();
	virtual ~BatterySensor();
	virtual SensorType get_type();

protected:
	virtual bool read_sensor(double &value);

};
