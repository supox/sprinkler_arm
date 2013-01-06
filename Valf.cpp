#include "Valf.h"
#include "Logger.h"
#include "TimeManager.h"

#include <stdio.h>

Valf::Valf(const int _id, const int _port_index) :
	id(_id),
	port_index(_port_index),
	is_open(false)
{
	if(port_index > 0) // Close the valf
	{
		is_open = true;
		SetState(false);
	}
}

Valf::~Valf()
{
}

bool Valf::SetState(const bool open)
{
    if(this->is_open != open) {
        // Mock placeholder
        this->is_open  = open;
        
        // log
        {
					char log_line[64];
					snprintf(log_line, 64, "Setting state of valf %d (port %d) to %d", id, port_index, is_open);
					Logger::AddLine(log_line, Logger::NORMAL);
        }
    }
    return true;
	
}

bool Valf::GetState(bool &open)
{
    // Mock placeholder
    open = this->is_open;
    return true;
}

bool Valf::do_instructions(Vector<ValfPtr> &valves, Vector<Irrigation> &irrigations)
{
	int current_time = TimeManager::GetSystemTime();
	const unsigned int number_of_valves = valves.size();
	const unsigned int number_of_irrigation = irrigations.size();
	if(number_of_valves == 0 || number_of_irrigation == 0)
		return true;
	
	for (unsigned int current_valf_index = 0 ; current_valf_index < number_of_valves ; current_valf_index++ )
	{
		Valf& v = *valves[current_valf_index];
		bool is_open = false;
		// TODO - build a hash instead of iterate over all irrigation every time...
		for (unsigned int current_irrigation_index = 0 ; current_irrigation_index < number_of_irrigation ; current_irrigation_index ++)
		{
			Irrigation& irrigation = irrigations[current_irrigation_index];
			if (v.id == irrigation.valf_id) {
					if(irrigation.mode == Irrigation::TIME) {
						is_open |=
										current_time >= irrigation.start_time &&
										current_time <= irrigation.start_time + irrigation.amount;
					} else if(irrigation.mode == Irrigation::VOLUME) {
							// TODO!!
							Logger::AddLine("valf_do_instructions - volume irrigation not implemented.", Logger::ERROR);
							return false;
					}
			}
		}
		v.SetState(is_open);
	}
	return true;
}
