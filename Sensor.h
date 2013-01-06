#pragma once
#include "Alarm.h"
#include "Vector.h"
#include "SensorType.h"
#include "ReadingData.h"
#include "SmartPointer.h"
#include "ITimeListener.h"
#include "ISensorListener.h"

class Sensor : public ITimeListener
{
public:
	Sensor(ISensorListener* listener = NULL);
	virtual ~Sensor();
	
	virtual SensorType GetType() = 0;
	bool ReportReadings();
	virtual void TimeNotification();
	void SetListener(ISensorListener* listener){m_listener = listener;}

	int id;
	int port_index;
	Vector<AlarmPtr> alarms;

protected:
	bool ReadSensor();
	virtual bool ReadSensorFromHardware(double &value) = 0;
	bool AddReadingIfNeeded(const bool will_alarm);
	bool ReportReadingData(const char *url, ReadingData& data);

	double last_reading_value;
	int last_reading_time;
	int last_saved_reading_time;
	int report_reading_time_delta;
	Vector<ReadingData> readings_to_report;
	ISensorListener* m_listener;

};

typedef SmartPointer<Sensor> SensorPtr;
