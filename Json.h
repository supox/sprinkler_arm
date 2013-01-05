#pragma once

#include "Sensor.h"
#include "Sprinkler.h"
#include "Vector.h"

namespace JSON
{
	bool parse_sensors(const char* json_buffer, Vector<SensorPtr>& sensors);
	bool parse_valves(const char* json_buffer, Vector<ValfPtr>& valves);
	bool parse_irrigations(const char* json_buffer, Vector<Irrigation>& irrigations);
	bool parse_sprinkler_configuration(const char* json_buffer, Sprinkler& sprinkler);
};
