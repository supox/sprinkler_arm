#include "Sensor.h"
#include "Config.h"
#include "Time.h"
#include "TimeManager.h"
#include "StringBuffer.h"
#include "Communication.h"

#include <stdio.h> // for sprintf
#include <string.h> // for strcmp

Sensor::Sensor(ISensorListener* listener /* = NULL */) :
	id(0),
	port_index(0),
	alarms(),
	last_reading_value(0),
	last_reading_time(0),
	last_saved_reading_time(0),
	report_reading_time_delta(DEFAULT_SENSOR_READING_TIME_DELTA_SECONDS),
	readings_to_report(),
	m_listener(listener)
{
	// Register as a listener to TimeManager to schedule time readings
	TimeManager::NotifyAt(this, TimeManager::GetSystemTime() + report_reading_time_delta);
}

Sensor::~Sensor()
{
}

bool Sensor::ReadSensor()
{
	double value = 0;
	bool ret = ReadSensorFromHardware(value);
	if (ret)
	{
		last_reading_value = value;
		last_reading_time = TimeManager::GetSystemTime();
		bool has_alarmed = false;		
		const unsigned int number_of_alarms = alarms.size();
		for(int index = 0; index < number_of_alarms ; index++)
		{
			if(alarms[index]->will_alarm(last_reading_value)) {
				has_alarmed = true;
				break;
			}
		}
		ret = AddReadingIfNeeded(has_alarmed);
		if(has_alarmed && m_listener != NULL)
			m_listener->OnAlarm(this);
	}
	return ret;	
}

void Sensor::TimeNotification()
{
	ReadSensor();
	
	// Roll
	TimeManager::NotifyAt(this, TimeManager::GetSystemTime() + report_reading_time_delta);
}

bool Sensor::AddReadingIfNeeded(const bool will_alarm)
{
	if(will_alarm || last_reading_time > last_saved_reading_time + report_reading_time_delta)
	{
		// Save reading to queue:
		ReadingData data;
		data.reading_time = last_reading_time;
		data.reading_value = last_reading_value;
		readings_to_report.Add(data);
		last_saved_reading_time = last_reading_time;
	}
	return true;
}

bool Sensor::ReportReadings()
{

	// Build URL
	char url[128]; // asprintf would be alot easier... sigh
	int number_of_elements = snprintf(url, 128, SENSOR_URL_FORMAT, id);
	if (number_of_elements < 0) // sprintf failed.
		return false;
	
	// Report all readings
	bool ret = true;
	const unsigned int number_of_reports = readings_to_report.size();
	for(unsigned int report_index = 0 ; report_index < number_of_reports ; report_index++)
	{
		if (!ReportReadingData(url, readings_to_report[report_index]))
			ret = false;
	}

	if(ret)
		readings_to_report.Clear();
	
	return ret;
}

bool Sensor::ReportReadingData(const char *url, ReadingData& data)
{
	StringBuffer request;
	StringBuffer response;
	char buf[128];

	// would be so much easier with asprintf...
	int str_length = snprintf(buf, 128, SENSOR_READING_JSON_FORMAT, data.reading_value, data.reading_time);
	if (str_length < 0) // sprintf failed
			return false;
	
	request.Write(buf,str_length);
	
	if(!Communication::PostWebPage(url, request, response))
		return false;
	
	return (strcmp(response.GetBuffer(), ACK_STRING) == 0);
}
