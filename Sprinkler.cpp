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
		irrigations(),
		m_valves_manager()
{
	load_config();
	
	// Start report loop
	TimeNotification(TimeManager::GetSystemTime());
}

Sprinkler::~Sprinkler()
{
}

void Sprinkler::OnAlarm(Sensor* s)
{
	has_alarmed = true;
}

unsigned int Sprinkler::get_next_task_time()
{
	if( last_report_time <= 0 || last_irrigation_load_time <= 0) // Try every minute until success
		return TimeManager::GetSystemTime() + 60;
    
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

void Sprinkler::do_tasks()
{
    // Read sensor state, report if needed.
    if(needs_to_report_reading()) {
        report_reading();
    }
    
    // load irrigation instruction if needed.
		bool ret = true;
    if(needs_to_load_irrigations()) {
        ret = load_irrigations_instructions();
    }
    
    // Update valves mode:
    if(ret)
			m_valves_manager.Update(valves, irrigations);
}

bool Sprinkler::needs_to_report_reading() {
    if (has_alarmed)
        return true;

    unsigned int current_time = TimeManager::GetSystemTime();
    return (current_time >= last_report_time + refresh_rate);
}

bool Sprinkler::needs_to_load_irrigations() {
    unsigned int current_time = TimeManager::GetSystemTime();
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

bool Sprinkler::load_time()
{
	bool ret;
	StringBuffer sb;

	Logger::AddLine("Sprinkler::load_time : Updating clock.", Logger::DUMP);
	ret = Communication::GetWebPage(TIME_URL, sb);
	if (ret) {
		unsigned int current_time = 0;
		ret = JSON::parse_time(sb.GetBuffer(), current_time);
		if(ret)
			TimeManager::SetSystemTime(current_time);
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
		ret = JSON::parse_sprinkler_configuration(sb.GetBuffer(), *this); // I'll go to hell bcz of this circular dependency.

	return ret;
}

bool Sprinkler::load_sensors_config() {
	StringBuffer sb;

	// Load sensors
	bool ret = Communication::GetWebPage(SENSORS_CONFIGURATION_URL, sb);
	if (ret)
			ret = JSON::parse_sensors(sb.GetBuffer(), sensors);
	if(ret)
	{
		unsigned int number_of_sensors = sensors.size();
		for(unsigned int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++)
			sensors[sensor_index]->SetListener(this);
	}
	
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

	ret &= load_time();
	ret &= load_sprinkler_config();
	ret &= load_sensors_config();
	ret &= load_valves_config();
	ret &= load_irrigations_instructions();
	if(ret)
		m_valves_manager.Update(valves, irrigations);

	return ret;
}

bool Sprinkler::report_reading() {
	bool ret = true;

	Logger::AddLine("Sprinkler::report_reading - Reporting readings.", Logger::DUMP);

	const unsigned int number_of_sensors = sensors.size();
	for (unsigned int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++ )
	{
		if(!sensors[sensor_index]->ReportReadings())
			ret = false;
	}

	if (ret) {
			last_report_time = TimeManager::GetSystemTime();
			has_alarmed = false; // reported about the alarm, set to false.
	}

	return ret;
}

void Sprinkler::TimeNotification(unsigned int time)
{
	if(needs_to_do_tasks())
		do_tasks();

	unsigned int next_notification = get_next_task_time();
	
	// Watch dog.
	next_notification = std::min(next_notification,TimeManager::GetSystemTime() + 3600);
	
	TimeManager::NotifyAt(this, next_notification);
}
