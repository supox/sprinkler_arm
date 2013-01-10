#pragma once

#include "Vector.h"
#include "Irrigation.h"
#include "SmartPointer.h"
#include "IrrigationList.h"

class Valf;
typedef SmartPointer<Valf> ValfPtr;

class Valf
{
public:
	Valf(const int id, const int port_index);	
	~Valf();

	virtual bool SetState(const bool is_open); // the virtual is for mocks.
	bool GetState(bool &is_open);	

	int id;
	int port_index;
	IrrigationList irrigation_list;

private:
	bool is_open;

};
