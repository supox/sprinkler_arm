#pragma once
#include "Sensor.h"
#include "IInputListener.h"
#include "ITask.h"

class WaterSensor : public Sensor, private IInputListener, private ITask
{
public:
	WaterSensor(const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
	virtual ~WaterSensor();
	virtual SensorType GetType();


protected:
	virtual bool ReadSensorFromHardware(double &value);
	virtual void OnInputChanged(unsigned int input_index, const bool input_value);
	virtual void DoTask();

};
