#pragma once

#include "Vector.h"
#include "Irrigation.h"
#include "SmartPointer.h"

class Valf;
typedef SmartPointer<Valf> ValfPtr;

class Valf
{
public:
	Valf(const int id, const int port_index);	
	~Valf();

	bool set_state(const bool is_open);
	bool get_state(bool &is_open);	

	static bool do_instructions(Vector<ValfPtr> &valves, Vector<Irrigation> &irrigations);

	int id;
	int port_index;

private:
	bool is_open;

};

