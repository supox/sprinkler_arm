#ifdef _TESTS

#include <stdio.h>
#include <stdlib.h>

#include "StringBuffer.h"
#include "Vector.h"
#include "JsonParser.h"
#include "Sprinkler.h"
#include "Valf.h"
#include "SensorFactory.h"

void TestJson()
{	
	StringBuffer sb;
	Valf v(0,0);
	
	Sensor* sensor = SensorFactory::CreateSensor(WATER_READER);
	delete(sensor);
	
	Sprinkler s;
	Vector<JSON::jsmntok_t> tokens;
	JSON::Parser parser;
	int jsmn_ret = parser.Parse(
	"[{\"start_time\":1350339360,\"valf_id\":4,\"irrigation_mode\":\"time\",\"amount\":2},{\"start_time\":1350339360,\"valf_id\":6,\"irrigation_mode\":\"time\",\"amount\":4},{\"start_time\":1350944160,\"valf_id\":4,\"irrigation_mode\":\"time\",\"amount\":2},{\"start_time\":1350944160,\"valf_id\":6,\"irrigation_mode\":\"volume\",\"amount\":4}]\n\n",
		tokens
	);
	
	if(jsmn_ret < 0)
	{
		// ERROR
		while(1);
	}

	// printf("Number of tokens = %d", tokens->size); 
 
}

#endif
