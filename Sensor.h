#pragma once
#include "Alarm.h"
#include "Vector.h"
#include "SensorType.h"
#include "ReadingData.h"
#include "SmartPointer.h"

class Sensor
{
public:
	Sensor();
	virtual ~Sensor();
	
	bool read_sensor(double &value, bool& will_alarm);
	virtual SensorType get_type() = 0;
	bool ReportReadings();

	int id;
	int port_index;
	Vector<AlarmPtr> alarms;

protected:
	virtual bool read_sensor(double &value) = 0;
	bool add_reading_if_needed(const bool will_alarm);
	bool ReportReadingData(const char *url, ReadingData& data);

	double last_reading_value;
	int last_reading_time;
	int last_saved_reading_time;
	int report_reading_time_delta;
	Vector<ReadingData> readings_to_report;

};

typedef SmartPointer<Sensor> SensorPtr;
