#include "Sensor.h"
#include "Config.h"
#include "Time.h"
#include "TimeManager.h"
#include "StringBuffer.h"
#include "Communication.h"

#include <stdio.h> // for sprintf
#include <string.h> // for strcmp

#define MAX_READINGS_NUMBER_BEFORE_REPORTING 0x200

Sensor::Sensor(const int _id, const int _port_index, const double sensor_value /* = 0 */, ISensorListener* listener /* = NULL */) :
	id(_id),
	port_index(_port_index),
	alarms(),
	last_reading_value(sensor_value),
	last_reading_time(0),
	last_saved_reading_time(0),
	report_reading_time_delta(DEFAULT_SENSOR_READING_TIME_DELTA_SECONDS),
	m_has_alarmed(false),
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
		ret = OnRead(value);
	}
	return ret;	
}

bool Sensor::OnRead(const double value)
{
	bool ret = true;
	bool value_different_from_last_value = last_reading_value != value;
	
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
	
	ret = AddReadingIfNeeded(has_alarmed, value_different_from_last_value);
	
	if(has_alarmed && m_listener != NULL)
		m_listener->OnAlarm(this);

	// set alarm signal
	m_has_alarmed = has_alarmed;
	
	return ret;
}

void Sensor::TimeNotification(unsigned int time)
{
	ReadSensor();
	
	// Roll
	TimeManager::NotifyAt(this, time + report_reading_time_delta);
}

bool Sensor::AddReadingIfNeeded(const bool has_alarm, const bool value_different_from_last_value)
{
	if(value_different_from_last_value || has_alarm && !m_has_alarmed || last_reading_time >= last_saved_reading_time + report_reading_time_delta)
	{
		// Save reading to queue:
		ReadingData data;
		data.reading_time = last_reading_time;
		data.reading_value = last_reading_value;
		readings_to_report.Add(data);
		last_saved_reading_time = last_reading_time;
		
		if(readings_to_report.size() >= MAX_READINGS_NUMBER_BEFORE_REPORTING) {
			if(m_listener!=NULL)
				m_listener->OnReportDataFull(this);
			readings_to_report.RemoveAt(0);
		}
		
		return true;
	}
	
	return true;
}

bool Sensor::ReportReadings()
{	
	const unsigned int number_of_reports = readings_to_report.size();
	if(number_of_reports == 0)
		return true;

	// Build URL
	char url[128]; // asprintf would be alot easier... sigh
	int number_of_elements = snprintf(url, 128, SENSOR_URL_FORMAT, id);
	if (number_of_elements < 0) // sprintf failed.
		return false;
	
	// Report all readings
	bool ret = true;
	CVector<ReadingData> failed_to_sent_readings;
	const size_t report_bulk = 30;
	for(size_t report_index = 0 ; report_index < number_of_reports ; report_index+=report_bulk)
	{
		size_t bulk_end_index = report_index + report_bulk;
		if(bulk_end_index >= number_of_reports)
			bulk_end_index = number_of_reports-1;
		
		if (!ReportReadingData(url, report_index, bulk_end_index)) {
			ret = false;
			for(size_t failed_index = report_index ; failed_index <= bulk_end_index ; failed_index++)
				failed_to_sent_readings.Add(readings_to_report[failed_index]);
		}
	}

	readings_to_report = failed_to_sent_readings;
	
	return ret;
}

bool Sensor::ReportReadingData(const char *url, const size_t report_start_index, const size_t report_end_index)
{
	StringBuffer request;
	StringBuffer response;
	char buf[128];

	request.Write(SENSOR_READINGS_PREFIX, sizeof(SENSOR_READINGS_PREFIX)-1);
	for(size_t report_index = report_start_index ; report_index <= report_end_index ; report_index++) {
		const ReadingData& data = readings_to_report[report_index];
		// would be so much easier with asprintf...
		int str_length = snprintf(buf, 128, SENSOR_READING_JSON_FORMAT, data.reading_value, data.reading_time);
		if (str_length < 0) // sprintf failed
			return false;
		request.Write(buf,str_length);
		if(report_index != report_end_index)
			request.Write(",", 1);
	}
	request.Write("]}", 2);
	
	if(!Communication::PostWebPage(url, request, response))
		return false;
	
	return (strcmp(response.GetBuffer(), ACK_STRING) == 0);
}

bool Sensor::UpdateFrom(Sensor* other)
{
	if(this->GetType() != other->GetType())
		return false;
	if(this->id != other->id || this->port_index != other->port_index)
		return false;

	this->report_reading_time_delta	= other->report_reading_time_delta;
	this->alarms = other->alarms;
	if(	this->last_reading_value < other->last_reading_value )
		this->last_reading_value = other->last_reading_value;

	return true;
}
