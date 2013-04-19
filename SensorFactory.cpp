#include "SensorFactory.h"
#include "MockSensor.h"
#include "BatterySensor.h"
#include "WaterSensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, const int id, const int port_index, ISensorListener* listener /* = NULL */)
	{
		switch(type)
		{
			case MOCK:
					return new MockSensor(id, port_index, listener);
			case BATTERY:
					return new BatterySensor(id, port_index, listener);
			case WATER_READER:
					return new WaterSensor(id, port_index, listener);
			default:
					return NULL;
		}
	}
}
