#include "WaterSensor.h"
#include "InputsManager.h"

WaterSensor::WaterSensor(const int id, const int port_index, ISensorListener* listener) : Sensor(id, port_index, listener)
{
	InputsManager::GetInputsManager()->AddInputListener(this);
}

WaterSensor::~WaterSensor()
{
}

SensorType WaterSensor::GetType()
{
	return WATER_READER;
}


void WaterSensor::OnInputChanged(unsigned int input_index, const bool input_value)
{
	if( !input_value && this->port_index == input_index ) 
	{
		// OnRead would increament reading
		OnRead(last_reading_value+1);
	}
}

bool WaterSensor::ReadSensorFromHardware(double &value)
{
	value = this->last_reading_value;
	return true;
}
