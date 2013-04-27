#pragma once
#include "Sensor.h"

class BatterySensor : public Sensor
{
public:
	BatterySensor(const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
	virtual ~BatterySensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
