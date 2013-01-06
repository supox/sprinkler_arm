#include "SensorFactory.h"
#include "MockSensor.h"
#include "BatterySensor.h"
#include "WaterSensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type, ISensorListener* listener /* = NULL */)
	{
		switch(type)
		{
			case MOCK:
					return new MockSensor(listener);
			case BATTERY:
					return new BatterySensor(listener);
			case WATER_READER:
					return new WaterSensor(listener);
			default:
					return NULL;
		}
	}
}
