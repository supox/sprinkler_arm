#ifdef _TESTS

#include <stdio.h>
#include <stdlib.h>

#include "Json.h"
#include "JsonParser.h"
#include "TestHelpers.h"

using namespace TestHelpers;

void TestJsonTime()
{
	unsigned int time=0;
	bool ret = JSON::parse_time("{\"Time\":1364825982}", time);
	assert(ret);
	assert(time==1364825982);
}

void TestJson()
{
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
