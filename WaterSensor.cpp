#include "WaterSensor.h"

WaterSensor::WaterSensor() : Sensor()
{
}

WaterSensor::~WaterSensor()
{
}

SensorType WaterSensor::get_type()
{
	return WATER_READER;
}

bool WaterSensor::read_sensor(double &value)
{
	// TODO!
	double current_reading = 1.5;
	this->last_reading_value += current_reading;
	value = this->last_reading_value;

	return true;
}
