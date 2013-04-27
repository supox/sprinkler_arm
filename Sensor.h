#pragma once
#include "Alarm.h"
#include "Vector.h"
#include "CVector.h"
#include "SensorType.h"
#include "ReadingData.h"
#include "SmartPointer.h"
#include "ITimeListener.h"
#include "ISensorListener.h"

class Sensor : protected ITimeListener
{
public:
	Sensor(const int id, const int port_index, const double sensor_value = 0, ISensorListener* listener = NULL);
	virtual ~Sensor();
	
	virtual SensorType GetType() = 0;
	bool ReportReadings();
	void SetListener(ISensorListener* listener){m_listener = listener;}

	virtual void TimeNotification(unsigned int time); // Reading cycle - the sensor read itself every minute

	// If the other is too different, return false.
	virtual bool UpdateFrom(Sensor* other);

	double GetLastReadingValue(){return last_reading_value;}
	unsigned int GetNumberOfReadingToReport(){return readings_to_report.size();}
	int GetReportReadingTimeDelta(){return report_reading_time_delta;}

	int id;
	int port_index;
	Vector<AlarmPtr> alarms;

protected:
	bool ReadSensor();
	bool OnRead(const double value);
	virtual bool ReadSensorFromHardware(double &value) = 0;
	bool AddReadingIfNeeded(const bool will_alarm, const bool value_different_from_last_value);
	bool ReportReadingData(const char *url, ReadingData& data);

	double last_reading_value;
	int last_reading_time;
	int last_saved_reading_time;
	int report_reading_time_delta;
	bool m_has_alarmed;
	CVector<ReadingData> readings_to_report;
	ISensorListener* m_listener;

};

typedef SmartPointer<Sensor> SensorPtr;
