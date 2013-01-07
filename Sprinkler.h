#pragma once
#include "Vector.h"
#include "Sensor.h"
#include "Valf.h"
#include "Irrigation.h"
#include "ITimeListener.h"
#include "ValvesManager.h"

class Sprinkler : public ITimeListener, public ISensorListener
{
public:
	Sprinkler();
	virtual ~Sprinkler();
	
	virtual void TimeNotification();
	virtual void OnAlarm(Sensor*);

	int id;
	unsigned int refresh_rate;
	int main_valf_delay;
	int main_valf;
	int last_report_time;
	int last_irrigation_load_time;

private:
	int get_next_task_time();
	bool needs_to_do_tasks();
	void do_tasks();
	bool needs_to_report_reading();
	bool needs_to_load_irrigations();
	bool load_irrigations_instructions();
	bool load_sprinkler_config();
	bool load_sensors_config();
	bool load_valves_config();
	bool load_config();
	bool report_reading();

	bool has_alarmed;

	Vector<SensorPtr> sensors;
	Vector<ValfPtr> valves;
	Vector<Irrigation> irrigations;
	ValvesManager m_valves_manager;
};
