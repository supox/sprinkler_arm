#pragma once
#include "ITimeListener.h"
#include "Vector.h"
#include "Valf.h"
#include "Irrigation.h"

class ValvesManager : public ITimeListener
{
public:
	ValvesManager();
	virtual ~ValvesManager();
	
	virtual void TimeNotification();
	void Update(Vector<ValfPtr>& valves, Vector<Irrigation> &irrigations);

private:
	void UpdateValvesState();

	Vector<ValfPtr>* m_valves;
	Vector<Irrigation>* m_irrigations;
};
