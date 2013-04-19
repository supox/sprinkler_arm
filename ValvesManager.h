#pragma once
#include "ITimeListener.h"
#include "Vector.h"
#include "Valf.h"
#include "Irrigation.h"
#include "IrrigationLogData.h"

class ValvesManager : private ITimeListener
{
public:
	ValvesManager();
	virtual ~ValvesManager();
	
	virtual void TimeNotification(unsigned int time);
	void Update(Vector<ValfPtr>& valves, Vector<Irrigation> &irrigations);
	bool ReportIrrigationLog(const char* url);

private:
	void UpdateValvesState(const unsigned int time);
	void BuildIrrigationLists();

	Vector<ValfPtr>* m_valves;
	Vector<Irrigation>* m_irrigations;
	Vector<IrrigationLogData> m_logs;
};
