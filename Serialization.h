#pragma once
#include "Sensor.h"
#include "Vector.h"

namespace Serialization
{	
	bool Save(Vector<SensorPtr>& sensors);
	bool Load(Vector<SensorPtr>& sensors);
	bool Update(Vector<SensorPtr>& sensors, size_t sensor_index_to_update);	
};
