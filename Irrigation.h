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
	Irrigation( const Irrigation& other );
	Irrigation(	const int start_time, const int valf_id, const IrrigationModes mode, const size_t amount);

	~Irrigation();
	Irrigation& operator = (const Irrigation& other);
	bool operator == (const Irrigation& other);

	int start_time;
	int valf_id;
	IrrigationModes mode;
	size_t amount;

};
