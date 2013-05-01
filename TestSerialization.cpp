#ifdef _TESTS_STORAGE
#include "TestHelpers.h"
#include "Serialization.h"
#include "SensorFactory.h"

#include <string.h>

using namespace TestHelpers;

void TestSerialization()
{
	bool ret;
	SensorPtr sensor1(SensorFactory::CreateSensor(BATTERY, 1, 2, 3, NULL));
	SensorPtr sensor2(SensorFactory::CreateSensor(WATER_READER, 4, 5, 6, NULL));

	Vector<SensorPtr> sensors;
	sensors.Add(sensor1);
	sensors.Add(sensor2);
	
	ret = Serialization::Save(sensors);
	assert(ret);
	
	
	Vector<SensorPtr> sensors_load;
	ret = Serialization::Load(sensors_load);
	assert(ret);
	
	assert(sensors_load.size() == 2);
	assert(sensors_load[0]->id == 1);
	assert(sensors_load[0]->port_index == 2);
	assert(sensors_load[0]->GetLastReadingValue() == 3);
	assert(sensors_load[0]->GetType() == BATTERY);
	assert(sensors_load[1]->id == 4);
	assert(sensors_load[1]->port_index == 5);
	assert(sensors_load[1]->GetLastReadingValue() == 6);
	assert(sensors_load[1]->GetType() == WATER_READER);

	// TODO - 
	//bool Update(Vector<SensorPtr>& sensors, size_t sensor_index_to_update);	

}
	
	

#endif
