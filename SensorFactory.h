#pragma once
#include "Sensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, ISensorListener* listener = NULL);
}
