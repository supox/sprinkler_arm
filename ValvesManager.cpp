#include "ValvesManager.h"
#include "TimeManager.h"
#include "Logger.h"
#include <algorithm> // For min

ValvesManager::ValvesManager() :
	m_valves(NULL), m_irrigations(NULL)
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
	int current_time = TimeManager::GetSystemTime();
	
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
			if (v.id == irrigation.valf_id) {
					if(irrigation.mode == Irrigation::TIME) {
						const int end_time = irrigation.start_time + irrigation.amount;
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
		v.SetState(is_open);
	}
	
	// Schedule next notification
	if(next_notification_time >= 0)
		TimeManager::NotifyAt(this, next_notification_time);
}
