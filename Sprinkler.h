#pragma once
#include "Vector.h"
#include "Sensor.h"
#include "Valf.h"
#include "Irrigation.h"
#include "ITimeListener.h"


class Sprinkler : public ITimeListener
{
public:
	Sprinkler();
	virtual ~Sprinkler();
	
	int get_next_task_time();
	bool needs_to_do_tasks();
	bool do_tasks();

	virtual void TimeNotification();

	int id;
	unsigned int refresh_rate;
	int main_valf_delay;
	int main_valf;
	int last_report_time;
	int last_irrigation_load_time;

private:
	bool needs_to_report_reading();
	bool needs_to_load_irrigations();
	bool read_sensors();
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
};
