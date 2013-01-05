#include "Sensor.h"
#include "Config.h"
#include "Time.h"
#include "TimeManager.h"
#include "StringBuffer.h"
#include "Communication.h"

#include <stdio.h> // for sprintf
#include <string.h> // for strcmp

Sensor::Sensor() :
	id(0),
	port_index(0),
	alarms(),
	last_reading_value(0),
	last_reading_time(0),
	last_saved_reading_time(0),
	report_reading_time_delta(DEFAULT_SENSOR_READING_TIME_DELTA_SECONDS),
	readings_to_report()
{
}

Sensor::~Sensor()
{
}

bool Sensor::read_sensor(double &value, bool& will_alarm)
{
	bool ret = read_sensor(value);
	if (ret)
	{
		last_reading_value = value;
		last_reading_time = TimeManager::GetSystemTime();
		ret = add_reading_if_needed(will_alarm);
		
		will_alarm = false;
		const unsigned int number_of_alarms = alarms.size();
		for(int index = 0; index < number_of_alarms ; index++)
		{
			if(alarms[index]->will_alarm(last_reading_value)) {
				will_alarm = true;
				break;
			}
		}
	}
	return ret;	
}

bool Sensor::add_reading_if_needed(const bool will_alarm)
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
	int number_of_elements = sprintf(url, SENSOR_URL_FORMAT, id);
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
