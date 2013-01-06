#include "WaterSensor.h"

WaterSensor::WaterSensor(ISensorListener* listener) : Sensor(listener)
{
}

WaterSensor::~WaterSensor()
{
}

SensorType WaterSensor::GetType()
{
	return WATER_READER;
}

bool WaterSensor::ReadSensorFromHardware(double &value)
{
	// TODO!
	double current_reading = 1.5;
	this->last_reading_value += current_reading;
	value = this->last_reading_value;

	return true;
}
