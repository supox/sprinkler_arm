#pragma once
#include "Sensor.h"
#include "IInputListener.h"

class WaterSensor : public Sensor, public IInputListener
{
public:
	WaterSensor(ISensorListener* listener = NULL);
	virtual ~WaterSensor();
	virtual SensorType GetType();
	virtual void OnInputChanged(unsigned int input_index);


protected:
	virtual bool ReadSensorFromHardware(double &value);

};
