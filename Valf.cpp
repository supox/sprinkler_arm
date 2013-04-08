#include "Valf.h"
#include "Logger.h"
#include "TimeManager.h"
#include "LedsManager.h"

#include <stdio.h>

Valf::Valf(const int _id, const int _port_index) :
	id(_id),
	port_index(_port_index),
	irrigation_list(),
	is_open(false)
{
	if(port_index >= 0) // Close the valf
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
		LedsManager::GetLedsManager()->SetLed(port_index, open);
		this->is_open = open;
			
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
	this->is_open = open = LedsManager::GetLedsManager()->GetLed(port_index);
	return true;
}
