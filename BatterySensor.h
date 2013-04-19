#pragma once
#include "Sensor.h"

class BatterySensor : public Sensor
{
public:
	BatterySensor(const int id, const int port_index, ISensorListener* listener = NULL);
	virtual ~BatterySensor();
	virtual SensorType GetType();

protected:
	virtual bool ReadSensorFromHardware(double &value);

};
