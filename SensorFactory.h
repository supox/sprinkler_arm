#pragma once
#include "Sensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
}
