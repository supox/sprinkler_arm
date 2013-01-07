#include "Irrigation.h"

Irrigation::Irrigation() :
	 start_time(0), valf_id(0) ,mode(TIME), amount(0)
{

}

Irrigation::Irrigation(	const int _start_time, const int _valf_id, const IrrigationModes _mode, const size_t _amount) :
	 start_time(_start_time), valf_id(_valf_id) ,mode(_mode), amount(_amount)
{
}

Irrigation::~Irrigation()
{
}

Irrigation& Irrigation::operator = (const Irrigation& other)
{
	if(this != & other)
	{
		start_time = other.start_time;
		valf_id = other.valf_id;
		mode = other.mode;
		amount = other.amount;
	}
	return *this;
}

