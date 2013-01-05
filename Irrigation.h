#pragma once
#include <stdlib.h>

class Irrigation
{
public:
	enum IrrigationModes {
		TIME,
		VOLUME
	};

	Irrigation();
	~Irrigation();
	Irrigation& operator = (const Irrigation& other);

	int start_time;
	int valf_id;
	IrrigationModes mode;
	size_t amount;

};
