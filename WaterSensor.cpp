#include "WaterSensor.h"
#include "InputsManager.h"

WaterSensor::WaterSensor(ISensorListener* listener) : Sensor(listener)
{
	InputsManager::AddInputListener(this);
}

WaterSensor::~WaterSensor()
{
}

SensorType WaterSensor::GetType()
{
	return WATER_READER;
}


void WaterSensor::OnInputChanged(unsigned int input_index)
{
	if( this->port_index == input_index ) 
	{
		// increament reading
		this->last_reading_value++;
		OnRead(last_reading_value);
	}
}

bool WaterSensor::ReadSensorFromHardware(double &value)
{
	value = this->last_reading_value;
	return true;
}
