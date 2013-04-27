#pragma once
#include "Sensor.h"
#include "IInputListener.h"

class WaterSensor : public Sensor, private IInputListener
{
public:
	WaterSensor(const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
	virtual ~WaterSensor();
	virtual SensorType GetType();
	virtual void OnInputChanged(unsigned int input_index, const bool input_value);


protected:
	virtual bool ReadSensorFromHardware(double &value);

};
