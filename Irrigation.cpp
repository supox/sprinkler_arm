#include "Irrigation.h"

Irrigation::Irrigation() :
	 start_time(0), valf_id(0) ,mode(TIME), amount(0)
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

