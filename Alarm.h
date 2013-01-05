#pragma once
#include "SmartPointer.h"

class Alarm
{
public:
	enum AlarmType
	{
		EQUAL,
		NOT_EQUAL,
		GREATER_THAN,
		LESS_THAN, 
		INVALID
	};

	Alarm(const double value, const AlarmType type);
	~Alarm();
	
	bool will_alarm(const double sensor_value);
	
	double alarm_value;
	AlarmType type;

};

typedef SmartPointer<Alarm> AlarmPtr;
