#include "Sprinkler.h"
#include "Config.h"
#include "TimeManager.h"
#include "Communication.h"
#include "Logger.h"
#include "Json.h"

#include <algorithm> // For max

Sprinkler::Sprinkler():
    id(0),refresh_rate(DEFAULT_REFRESH_RATE_SECONDS),
    main_valf_delay(0),
    main_valf(-1),
    last_report_time(0),
    last_irrigation_load_time(0),
    has_alarmed(false),
		sensors(),
		valves(),
		irrigations()
{
	load_config();
	
	// Start report loop
	TimeNotification();
}

Sprinkler::~Sprinkler()
{
}

int Sprinkler::get_next_task_time()
{
	// DEBUG - DELME
	// if( last_report_time <= 0 || last_irrigation_load_time <= 0)
	//    return TimeManager::GetSystemTime();
    
	return std::max(last_report_time, last_irrigation_load_time) + refresh_rate;
}

bool Sprinkler::needs_to_do_tasks()
{
    bool ret;
    ret =
        needs_to_report_reading() ||
        needs_to_load_irrigations();
    
    return ret;
}

bool Sprinkler::do_tasks()
{
    bool ret = true;
    
    // Read sensor state, report if needed.
    ret &= read_sensors();
    if(ret && needs_to_report_reading()) {
        ret &= report_reading();
    }
    
    // load irrigation instruction if needed.
    if(needs_to_load_irrigations()) {
        ret &= load_irrigations_instructions();
    }
    
    // Update valves mode:
    if(ret)
			ret &= Valf::do_instructions(valves, irrigations);
    
    return ret;

}

bool Sprinkler::read_sensors() {
	bool ret = true;
	const unsigned int number_of_sensors = sensors.size();
	for(int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++)
	{
		bool will_alarm = false;
		double value = 0;
		ret &= sensors[sensor_index]->read_sensor(value, will_alarm);
		has_alarmed |= will_alarm;
	}
	return ret;
}


bool Sprinkler::needs_to_report_reading() {
    if (has_alarmed)
        return true;

    int current_time = TimeManager::GetSystemTime();
    return (current_time >= last_report_time + refresh_rate);
}

bool Sprinkler::needs_to_load_irrigations() {
    int current_time = TimeManager::GetSystemTime();
    return (current_time >= last_irrigation_load_time + DEFAULT_IRRIGATIONS_REFRESH_TIME);
}


bool Sprinkler::load_irrigations_instructions() {
	// load irrigation definitions
	bool ret;
	StringBuffer sb;

	Logger::AddLine("sprinkler_load_irrigations : Loading irrigations instructions.", Logger::DUMP);
	ret = Communication::GetWebPage(SPRINKLER_IRRIGATION_URL, sb);
	if (ret) {
			ret = JSON::parse_irrigations(sb.GetBuffer(), irrigations);
			if(ret)
					last_irrigation_load_time = TimeManager::GetSystemTime();
	}

	if(!ret) {
			Logger::AddLine("sprinkler_load_irrigations : failed load irrigations instructions.", Logger::ERROR);
	}
	return ret;
}

bool Sprinkler::load_sprinkler_config()
{
	StringBuffer sb;
	bool ret = Communication::GetWebPage(SPRINKLER_CONFIGURATION_URL, sb);
	if (ret)
		ret = JSON::parse_sprinkler_configuration(sb.GetBuffer(), *this); // I'll go to hell due to this circular dependency.

	return ret;
}

bool Sprinkler::load_sensors_config() {
	StringBuffer sb;

	// Load sensors
	bool ret = Communication::GetWebPage(SENSORS_CONFIGURATION_URL, sb);
	if (ret)
			ret = JSON::parse_sensors(sb.GetBuffer(), sensors);

	return ret;
}

bool Sprinkler::load_valves_config() 
{
	StringBuffer sb;
	bool ret = Communication::GetWebPage(SPRINKLER_VALVES_URL, sb);
	if (ret)
			ret = JSON::parse_valves(sb.GetBuffer(), valves);

	return ret;
}

bool Sprinkler::load_config() {
    bool ret = true;
    
    ret &= load_sprinkler_config();
    ret &= load_sensors_config();
    ret &= load_valves_config();
    ret &= load_irrigations_instructions();
    
    return ret;
}

bool Sprinkler::report_reading() {
	bool ret = true;

	Logger::AddLine("sprinkler_report_reading : Reporting readings.", Logger::DUMP);

	const unsigned int number_of_sensors = sensors.size();
	for (unsigned int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++ )
	{
		if(!sensors[sensor_index]->ReportReadings())
			ret = false;
	}

	if (ret) {
			last_report_time = TimeManager::GetSystemTime();
			has_alarmed = false; // reported about the alarm, turn to false.
	}

	return ret;
}

void Sprinkler::TimeNotification()
{
	if(needs_to_do_tasks())
		do_tasks();

	int next_notification = get_next_task_time();
	
	// Watch dog.
	next_notification = std::min(next_notification,TimeManager::GetSystemTime() + 3600);
	
	TimeManager::NotifyAt(this, next_notification);
}
