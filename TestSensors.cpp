#ifdef _TESTS
#include "SensorFactory.h"
#include "InputsManager.h"
#include "ISensorListener.h"
#include "TestHelpers.h"
using namespace TestHelpers;

class MockSensorListener : public ISensorListener
{
public:
	MockSensorListener(): has_alarmed(false), data_full(false){}
	virtual ~MockSensorListener(){}
	virtual void OnAlarm(Sensor*) {has_alarmed = true;}
	virtual void OnReportDataFull(Sensor*) {data_full = true;}
	bool has_alarmed;
	bool data_full;
};

static void TestUpdateFrom()
{
	MockSensorListener listener;
	// Check update from:
	SensorPtr sensor1(SensorFactory::CreateSensor(WATER_READER, 1, 2, 3, &listener));
	SensorPtr sensor2(SensorFactory::CreateSensor(WATER_READER, sensor1->id, sensor1->port_index, 4, &listener));

	AlarmPtr alarm2(new Alarm(10, Alarm::LESS_THAN));
	sensor2->alarms.Add(alarm2);

	assert(sensor1->UpdateFrom(sensor2.get()));
	assert(sensor1->GetLastReadingValue() == sensor2->GetLastReadingValue());
	assert(sensor1->GetReportReadingTimeDelta() == sensor2->GetReportReadingTimeDelta());
	assert(sensor1->alarms == sensor2->alarms);

	SensorPtr sensor3(SensorFactory::CreateSensor(WATER_READER, sensor1->id, sensor1->port_index, 1, &listener));
	assert(sensor1->UpdateFrom(sensor3.get()));
	assert(sensor1->GetLastReadingValue() == 4 );
	assert(sensor1->GetReportReadingTimeDelta() == sensor3->GetReportReadingTimeDelta());
	assert(sensor1->alarms == sensor3->alarms);

	sensor2->id = sensor1->id+1;
	assert(!sensor1->UpdateFrom(sensor2.get()));

	sensor2->id = sensor1->id;
	sensor2->port_index = sensor1->port_index+1;
	assert(!sensor1->UpdateFrom(sensor2.get()));

	SensorPtr sensor4(SensorFactory::CreateSensor(BATTERY, sensor1->id, sensor1->port_index, 3, &listener));
	assert(!sensor1->UpdateFrom(sensor4.get()));
}

static void TestInitAndAlarms()
{
	MockSensorListener listener;
	const int port_index = 3;
	
	SensorPtr sensor(SensorFactory::CreateSensor(WATER_READER, 0, 0, 0, &listener));
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

static void TestDataOverflow()
{
	// Start with test vector:
	{
		CVector<ReadingData> readings_to_report;
		size_t i;
		for(i = 0 ; i < 0x100 ; i++) {
			ReadingData rd;
			bool ret =readings_to_report.Add(rd);
		}
		for( ; i < 1000 ; i++) {
			ReadingData rd;
			assert(readings_to_report.Add(rd));
		}
	}
	
	
	MockSensorListener listener;
	const int port_index = 3;
	// Check update from:
	SensorPtr sensor(SensorFactory::CreateSensor(WATER_READER, 1, port_index, 3, &listener));

	size_t i ;
	for(i = 0 ; i < 100000 && !listener.data_full ; i++) {
		InputsManager::NotifyListeners(port_index);	
	}
	assert(listener.data_full);
}

void TestSensors()
{
	TestInitAndAlarms();
	TestUpdateFrom();
	TestDataOverflow();
}

#endif
