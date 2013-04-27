#pragma once

class Sensor;

class ISensorListener
{
public:
	virtual ~ISensorListener(){}
	virtual void OnAlarm(Sensor*) = 0;
	virtual void OnReportDataFull(Sensor*) = 0;
};
