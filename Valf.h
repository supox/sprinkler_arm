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

	virtual bool SetState(const bool is_open); // the virtual is for mock states.
	bool GetState(bool &is_open);	

	static bool do_instructions(Vector<ValfPtr> &valves, Vector<Irrigation> &irrigations);

	int id;
	int port_index;

private:
	bool is_open;

};

