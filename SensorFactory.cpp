#include "SensorFactory.h"
#include "MockSensor.h"
#include "BatterySensor.h"
#include "WaterSensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, const int id, const int port_index, const double sensor_value /* = 0 */, ISensorListener* listener /* = NULL */)
	{
		switch(type)
		{
			case MOCK:
					return new MockSensor(id, port_index, sensor_value, listener);
			case BATTERY:
					return new BatterySensor(id, port_index, sensor_value, listener);
			case WATER_READER:
					return new WaterSensor(id, port_index, sensor_value, listener);
			default:
					return NULL;
		}
	}
}
