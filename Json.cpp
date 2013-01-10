#include "JsonParser.h"
#include "Json.h"
#include "Logger.h"
#include "SensorFactory.h"
#include <stdio.h>
#include <string.h>

#define SENSORS_TAG             "sensors"
#define ID_TAG                  "id"
#define ALARM_TAG               "alarms"
#define TIME_TAG               	"time"
#define PORT_INDEX_TAG          "port_index"
#define ALARM_VALUE_TAG         "alarm_value"
#define CONDITION_TYPE_TAG      "condition_type"
#define MAX_NUMBER_OF_TOKENS    100
#define REFRESH_RATE_TAG        "refresh_rate_seconds"
#define MAIN_VALF_DELAY_TAG     "main_valve_delay"
#define MAIN_VALF_TAG           "main_valf"

#define START_TIME_TAG          "start_time"
#define VALF_ID_TAG             "valf_id"
#define IRRIGATION_MODE_TAG     "irrigation_mode"
#define AMOUNT_TAG              "amount"

#define IRRIGATION_MODE_VOLUME  "volume"
#define IRRIGATION_MODE_TIME    "time"

#define SENSOR_TYPE_TAG					"type"
#define SENSOR_TYPE_WATER				"water_meter"
#define SENSOR_TYPE_BATTERY			"battery"
#define SENSOR_TYPE_TEMPERATURE	"temperature"
#define SENSOR_TYPE_PRESSURE		"pressure"
#define SENSOR_TYPE_HUMIDITY		"humidity"
#define SENSOR_TYPE_FERT				"fert_meter"
#define SENSOR_TYPE_DIFFERENTIAL_PRESSURE				"differential_pressure"

// Useful macro to compare between the string of the tag (t) and a string (s).
#define TOKEN_STRING(js, t, s) \
	(strncmp(js+(t).start, s, (t).end - (t).start) == 0 \
	 && strlen(s) == (t).end - (t).start)

namespace JSON
{	
	bool parse_sensors_internal(Vector<jsmntok_t>& tokens_vector, const char* json_buffer, Vector<SensorPtr>& sensors);
	bool parse_alarms_internal(Vector<jsmntok_t>& tokens_vector, const char* json_buffer, Vector<SensorPtr>& sensors);
	
	int find_json_array_token(Vector<jsmntok_t> &tokens, const char* json_buffer, const char* token_key) {
			size_t current_token_index, found_token_index;
			const size_t number_of_tokens = tokens.size();
			jsmntok_t* current_token = &tokens[0];
			for (current_token_index = 0; current_token_index < number_of_tokens; current_token_index++, current_token++) {
					if (current_token->type == JSMN_STRING && TOKEN_STRING(json_buffer, *current_token, token_key)) {
							found_token_index = current_token_index + 1;
							if ((current_token+1)->type != JSMN_ARRAY)
									return -1;
							else
									return found_token_index;
					}
			}
			return -1;
	}

	bool token_to_int(const char* json_buffer, jsmntok_t *token, int &value) {
			if (token->type != JSMN_PRIMITIVE)
					return false;

			return (sscanf(&json_buffer[token->start], "%d", &value) == 1);
	}

	bool token_to_uint(const char* json_buffer, jsmntok_t *token, size_t &value) {
			if (token->type != JSMN_PRIMITIVE)
					return false;

			if(strncmp(&json_buffer[token->start], "null",4) == 0)
			{
				value=0;
				return true;
			}
			
			return (sscanf(&json_buffer[token->start], "%zu", &value) == 1);
			
	}
	bool token_to_double(const char* json_buffer, jsmntok_t *token, double &value) {
			if (token->type != JSMN_PRIMITIVE)
					return false;

			return (sscanf(&json_buffer[token->start], "%lf", &value) == 1);
	}

	bool token_to_alarm_type(const char* json_buffer, jsmntok_t *token, Alarm::AlarmType &type) {
			const char* buf = &json_buffer[token->start];
			const size_t len = token->end - token->start;

			if (token->type != JSMN_STRING)
					return false;

			if (strncasecmp(buf, "EQUAL", len) == 0) {
					type = Alarm::EQUAL;
					return true;
			}
			if (strncasecmp(buf, "NOT_EQUAL", len) == 0) {
					type = Alarm::NOT_EQUAL;
					return true;
			}
			if (strncasecmp(buf, "GREATER_THAN", len) == 0) {
					type = Alarm::GREATER_THAN;
					return true;
			}
			if (strncasecmp(buf, "LESS_THAN", len) == 0) {
					type = Alarm::LESS_THAN;
					return true;
			}

			return false;
	}
	bool parse_sprinkler_configuration(const char* json_buffer, Sprinkler& sprinkler)
	{
    int current_token_index;
    Vector<jsmntok_t> tokens;
    jsmntok_t* current_token;
    size_t number_of_tokens;
		Parser parser;
    bool ret = true;

    // Parse json string
    if (parser.Parse(json_buffer, tokens) != JSON::JSMN_SUCCESS) {
        Logger::AddLine("json_parse_sprinkler_configuration: Could not parse json.", Logger::ERROR);
        return false;
    }
    
    current_token = &tokens[0];
    number_of_tokens = current_token->size;
    if (number_of_tokens <= 0 || current_token->type != JSMN_OBJECT) {
        Logger::AddLine("json_parse_sprinkler_configuration: Could not parse json.", Logger::ERROR);
        return false;
    }
    // iterate of all tokens, try to load configuration values
    current_token++;
    for ( current_token_index = 0; current_token_index < number_of_tokens ; ) {
        // Lets find out what to do with the key-
        if (TOKEN_STRING(json_buffer, *current_token, ID_TAG)) { // Id tag
            if (!token_to_int(json_buffer, (current_token+1), sprinkler.id)) {
                return false;
            }
        } else if (TOKEN_STRING(json_buffer, *current_token, REFRESH_RATE_TAG)) { // Port index tag
            if (!token_to_uint(json_buffer, (current_token+1), sprinkler.refresh_rate)) {
                return false;
            }
        } else if (TOKEN_STRING(json_buffer, *current_token, MAIN_VALF_DELAY_TAG)) { // Port index tag
            if (!token_to_int(json_buffer, (current_token+1), sprinkler.main_valf_delay)) {
                return false;
            }
        } else if (TOKEN_STRING(json_buffer, *current_token, MAIN_VALF_TAG)) { // Port index tag
            if (!token_to_int(json_buffer, (current_token+1), sprinkler.main_valf)) {
                return false;
            }
        } // else - ignore this key.
        current_token_index += 2;
        current_token += 2;
    }
    
    return ret;
}

	bool parse_sensors(const char* json_buffer, Vector<SensorPtr>& sensors)
	{
			bool ret = true;
			Vector<jsmntok_t> tokens;
			Parser parser;
			// Parse json string
			if (parser.Parse(json_buffer, tokens) == JSON::JSMN_SUCCESS) {
					if (!parse_sensors_internal(tokens, json_buffer, sensors)) {
							Logger::AddLine("json_parse_sensors: Could not find sensors data.", Logger::ERROR);
							ret = false;
					}

					if (!parse_alarms_internal(tokens, json_buffer, sensors)) {
							Logger::AddLine("json_parse_sensors: Could not find alarms data.", Logger::ERROR);
							ret = false;
					}        
			}
			else {
					ret = false;
					Logger::AddLine("json_parse_sensors: Could not parse json.", Logger::ERROR);
			}
			return ret;
	}

	bool parse_valves(const char* json_buffer, Vector<ValfPtr>& valves) {
			Vector<jsmntok_t> tokens;
			jsmntok_t* current_token;
			size_t number_of_tokens;
			int current_token_index;
			int valf_index;
			Parser parser;

			// Parse json string
			if (parser.Parse(json_buffer, tokens) != JSON::JSMN_SUCCESS) {
					Logger::AddLine("json_parse_valves: Could not parse json.", Logger::ERROR);
					return false;
			}

			// expected json : [{"id":4,"port_index":1},{"id":6,"port_index":2},{"id":5,"port_index":4}]
			current_token = &tokens[0];
			number_of_tokens = current_token->size;
			if (number_of_tokens <= 0 || current_token->type != JSMN_ARRAY) {
					Logger::AddLine("json_parse_valves: Could not parse json.", Logger::ERROR);
					return false;
			}
			
			current_token++;
			current_token_index = 1;
			
			// iterate of all tokens, try to build valves
			for (valf_index = 0 ; valf_index < number_of_tokens; valf_index++) {
					const int next_valf_token_index = current_token_index+current_token->size;
					int port_index =-1, id=-1, value;
					if(current_token->type != JSMN_OBJECT ) {
							current_token += current_token->size + 1;
							continue;
					}
					current_token_index++;
					current_token++;
					
					while(current_token_index < next_valf_token_index) {
							const int next_object_token = current_token_index + current_token->size+1;
							if (current_token->type != JSMN_STRING) {// Must be an error...
									current_token_index = next_object_token;
									current_token = &tokens[current_token_index];
									continue;
							}

							if ((current_token+1)->type != JSMN_PRIMITIVE) {// Must be an error...
									current_token_index = next_object_token;
									current_token = &tokens[current_token_index];
									continue;
							}

							// Read the value
							if (!token_to_int(json_buffer, current_token+1, value)) {
									current_token_index = next_object_token;
									current_token = &tokens[current_token_index];
									continue;
							}

							if (TOKEN_STRING(json_buffer, *current_token, ID_TAG)) { // Id tag
									id = value;
							} else if (TOKEN_STRING(json_buffer, *current_token, PORT_INDEX_TAG)) { // Port index tag
									port_index = value;
							} // else - ignore this key.

							current_token_index += 2;
							current_token += 2;
					}
					
					if(id>=0 && port_index>=0) {
							// Create valf
							ValfPtr v(new Valf(id, port_index));
							valves.Add(v);
					}
			}
			return true;
	}

	bool parse_irrigations(const char* json_buffer, Vector<Irrigation>& irrigations) {
		Vector<jsmntok_t> tokens;
		jsmntok_t* current_token;
		size_t number_of_tokens;
			int current_token_index;
			int irrigation_index;
		Parser parser;

			// Parse json string
			if (parser.Parse(json_buffer, tokens) != JSON::JSMN_SUCCESS) {
					Logger::AddLine("json_parse_irrigations: Could not parse json.", Logger::ERROR);
					return false;
			}

			// expected json : [{"start_time":1350339360,"valf_id":4,"irrigation_mode":"time","amount":2}]
			current_token = &tokens[0];
			number_of_tokens = current_token->size;
			if (number_of_tokens <= 0 || current_token->type != JSMN_ARRAY) {
					Logger::AddLine("json_parse_irrigations: Could not parse json.", Logger::ERROR);
					return false;
			}
			
			current_token++;
			current_token_index = 1;
			
			// iterate of all tokens, try to build valves
			for (irrigation_index = 0 ; irrigation_index < number_of_tokens; irrigation_index++) {
					const int next_irrigation_token_index = current_token_index+current_token->size;
					int number_of_processed_tokens = 0;
					int start_time=0;
					int valf_id=-1;
					Irrigation::IrrigationModes mode = Irrigation::TIME;
					size_t amount = 0;
					
					if(current_token->type != JSMN_OBJECT ) { // Not an object, skip to the next token.
							current_token += current_token->size + 1;
							continue;
					}
					current_token_index++;
					current_token++;
					
					while(current_token_index < next_irrigation_token_index) {
							const int next_object_token = current_token_index + current_token->size+1;
							if (current_token->type != JSMN_STRING) {// Must be an error...
									current_token_index = next_object_token;
									current_token = &tokens[current_token_index];
									continue;
							}

							if ((current_token+1)->type == JSMN_PRIMITIVE) {
									int value;
									// Read the value
									if (!token_to_int(json_buffer, current_token+1, value)) { // Error
											current_token_index = next_object_token;
											current_token = &tokens[current_token_index];
											continue;
									}

									if (TOKEN_STRING(json_buffer, *current_token, START_TIME_TAG)) {
											start_time = value;
											number_of_processed_tokens++;
									} else if (TOKEN_STRING(json_buffer, *current_token, VALF_ID_TAG)) {
											valf_id = value;
											number_of_processed_tokens++;
									} else if (TOKEN_STRING(json_buffer, *current_token, AMOUNT_TAG)) {
											amount = value;
											number_of_processed_tokens++;
									} // else - ignore this key.
							}
							else if ((current_token+1)->type == JSMN_STRING) {
									if (TOKEN_STRING(json_buffer, *current_token, IRRIGATION_MODE_TAG)) {
											if (TOKEN_STRING(json_buffer, *(current_token+1), IRRIGATION_MODE_TIME)) {
													mode = Irrigation::TIME;
													number_of_processed_tokens++;
											} else if (TOKEN_STRING(json_buffer, *(current_token+1), IRRIGATION_MODE_VOLUME)) {
													mode = Irrigation::VOLUME;
													number_of_processed_tokens++;
											}
											// TODO - what to do in case of an error? - currently ignore.
									} // else - ignore this key.
							}
							else { // Error
									current_token_index = next_object_token;
									current_token = &tokens[current_token_index];
									continue;                
							}

							current_token_index += 2;
							current_token += 2;
					}
					
					if(number_of_processed_tokens >= 4) {
							// Create valf
							Irrigation i;
							i.amount = amount;
							i.mode = mode;
							i.start_time = start_time;
							i.valf_id = valf_id;
											
							irrigations.Add(i);
					}
			}
			
			return true;
	}

	bool parse_sensors_internal(Vector<jsmntok_t>& tokens_vector, const char* json_buffer, Vector<SensorPtr>& sensors)
	{
			int sensors_array_token_index, current_token_index, sensor_index;
			jsmntok_t* tokens = &tokens_vector[0];

			// Find sensors array token:
			sensors_array_token_index = find_json_array_token(tokens_vector, json_buffer, SENSORS_TAG);
			if (sensors_array_token_index < 0) {
					return false;
			}

			current_token_index = sensors_array_token_index + 1;
			
			// iterate of all tokens, try to build sensors
			for (sensor_index = 0 ; sensor_index < tokens[sensors_array_token_index].size; sensor_index++) {
					int id = -1, port_index = -1, value;
					SensorType mode = MOCK;
					const unsigned int next_sensor_token_index = current_token_index + tokens[current_token_index].size + 1;

					// We're expecting something like - {"id":4,"port_index":6, "type":"water_meter"}
					if (tokens[current_token_index].type != JSMN_OBJECT || tokens[current_token_index].size < 4) {
							current_token_index = next_sensor_token_index;
							continue;
					}

					current_token_index++;

					// First token is the key, the second is the value
					
					while (current_token_index < next_sensor_token_index) {
							const unsigned int next_object_token_index = current_token_index + tokens[current_token_index].size + 1;
							if (tokens[current_token_index].type != JSMN_STRING) { // Must be an error...
									current_token_index = next_object_token_index;
									continue;
							}

							if (tokens[current_token_index + 1].type == JSMN_STRING) { // probably type
									if (TOKEN_STRING(json_buffer, tokens[current_token_index], SENSOR_TYPE_TAG)) {
											if (TOKEN_STRING(json_buffer, tokens[current_token_index + 1], SENSOR_TYPE_WATER)) {
													mode = WATER_READER;
											} else if (TOKEN_STRING(json_buffer, tokens[current_token_index + 1], SENSOR_TYPE_BATTERY)) {
													mode = BATTERY;
											}
											// TODO - add other sensor types.
											
											// TODO - what to do in case of an error? - currently ignore.
									} // else - ignore this key.
							}
							else if (tokens[current_token_index + 1].type == JSMN_PRIMITIVE) {
								// Read the value
								if (!token_to_int(json_buffer, &tokens[current_token_index + 1], value)) {
										current_token_index = next_object_token_index;
										continue;
								}

								if (TOKEN_STRING(json_buffer, tokens[current_token_index], ID_TAG)) { // Id tag
										id = value;
								} else if (TOKEN_STRING(json_buffer, tokens[current_token_index], PORT_INDEX_TAG)) { // Port index tag
										port_index = value;
								} // else - ignore this key.
							}
							current_token_index += 2;
					}

					if (id >= 0 && port_index >= 0) { // Add sensor
							SensorPtr sensor(SensorFactory::CreateSensor(mode)); // TODO - add type.
							sensor->id = id;
							sensor->port_index = port_index;
							sensors.Add(sensor);
					}
			}
			return true;
	}

	bool parse_alarms_internal(Vector<jsmntok_t>& tokens_vector, const char* json_buffer, Vector<SensorPtr>& sensors)
	{
			int alarm_token_index, current_token_index, alarm_index;
			jsmntok_t* tokens = &tokens_vector[0];

			if (sensors.empty())
					return true;

			alarm_token_index = find_json_array_token(tokens_vector, json_buffer, ALARM_TAG);
			if (alarm_token_index < 0) { // No alarms, should not be a problem.
					return true;
			}

			if(tokens[alarm_token_index].type != JSMN_ARRAY)
					return false;
			
			// Build sensor hash table - port_index to sensor;
			unsigned int max_port_index = 0;
			const unsigned int number_of_sensors = sensors.size();
			for (unsigned int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++)
			{
					SensorPtr sensor = sensors[sensor_index];
					if (sensor->port_index < 0)
							return false; // invalid

					if (sensor->port_index > max_port_index)
							max_port_index = sensor->port_index;
			}

			SensorPtr* sensors_hash = new SensorPtr[max_port_index + 1];
			for (unsigned int sensor_index = 0 ; sensor_index < number_of_sensors ; sensor_index++)
			{
					sensors_hash[sensors[sensor_index]->port_index] = sensors[sensor_index];
			}

			// iterate of all tokens, try to build alarms
			for (current_token_index = alarm_token_index + 1, alarm_index = 0;
							alarm_index < tokens[alarm_token_index].size; alarm_index++ ) {
					int port_index = -1;
					double alarm_value = -9999;
					Alarm::AlarmType alarm_type = Alarm::INVALID;
					const size_t number_of_object_tokens = tokens[current_token_index].size;
					const size_t next_alarm_token_index = current_token_index + number_of_object_tokens; 

					// We're expecting something like - {"port_index":1,"alarm_value":5.0,"condition_type":"greater_than"}
					if (tokens[current_token_index].type != JSMN_OBJECT || number_of_object_tokens < 6) {
							current_token_index = next_alarm_token_index;
							continue; // TODO - add logs
					}

					current_token_index++;

					// First token is the key, the second is the value
					while (current_token_index < next_alarm_token_index) {
							const unsigned int next_object_token_index = current_token_index + tokens[current_token_index].size + 1;
							if (tokens[current_token_index].type != JSMN_STRING) {// Must be an error...
									current_token_index = next_object_token_index;
									continue;
							}

							if (tokens[current_token_index + 1].type != JSMN_PRIMITIVE
											&& tokens[current_token_index + 1].type != JSMN_STRING) {// Must be an error...
									current_token_index = next_object_token_index;
									continue;
							}

							if (TOKEN_STRING(json_buffer, tokens[current_token_index], ALARM_VALUE_TAG)) { // alarm value tag
									if (!token_to_double(json_buffer, &tokens[current_token_index + 1], alarm_value)) {
											current_token_index = next_alarm_token_index;
											break;
									}
							} else if (TOKEN_STRING(json_buffer, tokens[current_token_index], PORT_INDEX_TAG)) { // Port index tag
									if (!token_to_int(json_buffer, &tokens[current_token_index + 1], port_index)) {
											current_token_index = next_alarm_token_index;
											break;
									}
							} else if (TOKEN_STRING(json_buffer, tokens[current_token_index], CONDITION_TYPE_TAG)) { // Condition type tag
									if (!token_to_alarm_type(json_buffer, &tokens[current_token_index + 1], alarm_type)) {
											current_token_index = next_alarm_token_index;
											break;
									}
							} // else - ignore this key.

							current_token_index += 2;
					}

					if (port_index >= 0 && alarm_type != Alarm::INVALID && alarm_value != -9999 && port_index <= max_port_index) { // Add alarm
							AlarmPtr alarm(new Alarm(alarm_value, alarm_type));
							SensorPtr s = sensors_hash[port_index];
							if(s.get()!=NULL)
									s->alarms.Add(alarm);
					}
			}

			delete[](sensors_hash);

			return true;
	}

	bool parse_time(const char* json_buffer, int &time)
	{
    int current_token_index;
    Vector<jsmntok_t> tokens;
    jsmntok_t* current_token;
    size_t number_of_tokens;
		Parser parser;
    bool ret = true;

    // Parse json string
    if (parser.Parse(json_buffer, tokens) != JSON::JSMN_SUCCESS) {
        Logger::AddLine("json_parse_time: Could not parse json.", Logger::ERROR);
        return false;
    }
    
    current_token = &tokens[0];
    number_of_tokens = current_token->size;
    if (number_of_tokens <= 0 || current_token->type != JSMN_OBJECT) {
        Logger::AddLine("json_parse_time: Could not parse json.", Logger::ERROR);
        return false;
    }
    // iterate of all tokens, try to load configuration values
    current_token++;
    for ( current_token_index = 0; current_token_index < number_of_tokens ; )
		{
        // Lets find out what to do with the key-
        if (TOKEN_STRING(json_buffer, *current_token, TIME_TAG)) { // Id tag
            if (!token_to_int(json_buffer, (current_token+1), time)) {
                return true;
            }
        } // else - ignore this key.
        current_token_index += 2;
        current_token += 2;
    }
    
    return ret;
		
	}	
};

