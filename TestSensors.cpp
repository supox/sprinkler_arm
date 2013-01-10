#ifdef _TESTS
#include "SensorFactory.h"
#include "InputsManager.h"
#include "ISensorListener.h"
#include "TestHelpers.h"
using namespace TestHelpers;


class MockSensorListener : public ISensorListener
{
public:
	MockSensorListener(): has_alarmed(false){}
	virtual ~MockSensorListener(){}
	virtual void OnAlarm(Sensor*) {has_alarmed = true;}
	bool has_alarmed;
};

void TestSensors()
{
	MockSensorListener listener;
	const int port_index = 3;
	
	SensorPtr sensor(SensorFactory::CreateSensor(WATER_READER, &listener));
	assert(sensor.get()!=NULL);
	assert(sensor->GetType() == WATER_READER);
	assert(sensor->GetLastReadingValue() == 0);
	sensor->port_index = port_index;
	
	// Test notification
	InputsManager::NotifyListeners(port_index);	
	assert(sensor->GetLastReadingValue() == 1);

	InputsManager::NotifyListeners(port_index);	
	assert(sensor->GetLastReadingValue() == 2);
	
	InputsManager::NotifyListeners(port_index+1);	
	assert(sensor->GetLastReadingValue() == 2);
	assert(listener.has_alarmed == false);
	
	// Check alarms
	AlarmPtr alarm(new Alarm(10, Alarm::LESS_THAN));
	sensor->alarms.Add(alarm);

	InputsManager::NotifyListeners(port_index);	
	assert(sensor->GetLastReadingValue() == 3);
	assert(listener.has_alarmed == true);


}

#endif
