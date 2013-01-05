#include "SensorFactory.h"
#include "MockSensor.h"
#include "BatterySensor.h"
#include "WaterSensor.h"

namespace SensorFactory
{
	Sensor* CreateSensor(const SensorType type)
	{
		switch(type)
		{
			case MOCK:
					return new MockSensor();
			case BATTERY:
					return new BatterySensor();
			case WATER_READER:
					return new WaterSensor();
			default:
					return NULL;
		}
	}
}
