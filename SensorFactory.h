#pragma once
#include "Sensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, const int id, const int port_index, ISensorListener* listener = NULL);
}
