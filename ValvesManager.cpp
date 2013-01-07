#include "ValvesManager.h"
#include "TimeManager.h"
#include "Logger.h"
#include "Communication.h"
#include "Config.h"

#include <stdio.h> // for sprintf
#include <string.h> // for strcmp
#include <algorithm> // For min

ValvesManager::ValvesManager() :
	m_valves(NULL), m_irrigations(NULL), m_logs()
{
}

ValvesManager::~ValvesManager()
{
}
	
void ValvesManager::TimeNotification()
{
	UpdateValvesState();
}

void ValvesManager::Update(Vector<ValfPtr>& valves, Vector<Irrigation> &irrigations)
{
	m_valves = &valves;
	m_irrigations = &irrigations;
	
	// next time notification is no longer valid
	TimeManager::RemoveNotifications(this);
	
	// recompute valves state
	UpdateValvesState();
}

void ValvesManager::UpdateValvesState()
{
	const int current_time = TimeManager::GetSystemTime();
	
	const unsigned int number_of_valves = m_valves->size();
	const unsigned int number_of_irrigation = m_irrigations->size();
	if(number_of_valves == 0 || number_of_irrigation == 0)
		return; // Nothing to do.
	
	int next_notification_time = -1;
	for (unsigned int current_valf_index = 0 ; current_valf_index < number_of_valves ; current_valf_index++ )
	{
		Valf& v = *(*m_valves)[current_valf_index];
		bool is_open = false;
		// TODO - build a hash instead of iterate over all irrigation every time...
		for (unsigned int current_irrigation_index = 0 ; current_irrigation_index < number_of_irrigation ; current_irrigation_index ++)
		{
			Irrigation& irrigation = (*m_irrigations)[current_irrigation_index];
			if(irrigation.amount <=0)
			{
				Logger::AddLine("ValvesManager::UpdateValvesState - amount of zero. ignore.", Logger::ERROR);
				continue;
			}
			
			if (v.id == irrigation.valf_id) {
					if(irrigation.mode == Irrigation::TIME) {
						const int end_time = irrigation.start_time + irrigation.amount -1;
						is_open |=
										current_time >= irrigation.start_time &&
										current_time <= end_time;
						if(irrigation.start_time > current_time)
						{
							if(irrigation.start_time > next_notification_time || next_notification_time < 0)
								next_notification_time = irrigation.start_time;
						}
						else if(end_time > current_time)
						{
							if(end_time > next_notification_time || next_notification_time < 0)
								next_notification_time = end_time;
						}
						
					} else if(irrigation.mode == Irrigation::VOLUME) {
							// TODO!!
							Logger::AddLine("valf_do_instructions - volume irrigation not implemented.", Logger::ERROR);
					}
			}
		}
		
		// Add to log
		bool was_open = false;
		if(v.GetState(was_open) && was_open != is_open)
		{
			IrrigationLogData data;
			data.time = current_time;
			data.valf_id = v.id;
			data.valf_open = is_open;
			m_logs.Add(data);
		}
		
		v.SetState(is_open);
	}
	
	// Schedule next notification
	if(next_notification_time >= 0)
		TimeManager::NotifyAt(this, next_notification_time);
}

bool ValvesManager::ReportIrrigationLog(const char* url)
{
	if(m_logs.empty())
		return true;
	StringBuffer request;
	StringBuffer response;
	request.Write("[",1);
	char buf[128];
	const unsigned int number_of_logs = m_logs.size();
	for(unsigned int log_index = 0 ; log_index < number_of_logs ; log_index++)
	{
		IrrigationLogData& data = m_logs[log_index];
		int number_of_chars = snprintf(buf,128, "{\"time\":%d, \"valf_id\":%d, \"valf_open\":%d}", data.time, data.valf_id, data.valf_open);
		if(number_of_chars<0)
			return false;
		request.Write(buf, number_of_chars);
	}
	request.Write("]",1);
	
	if(!Communication::PostWebPage(url, request, response))
		return false;
	
	return (strcmp(response.GetBuffer(), ACK_STRING) == 0);

}
