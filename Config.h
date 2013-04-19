#pragma once

// Constants
#define MAX_NUMBER_OF_SENSORS 16
#define MAX_SENSORS_READING_SIZE 512
#define QUEUE_MAX_NUMBER_OF_ELEMENTS 256
#define DEFAULT_REFRESH_RATE_SECONDS 3600 // every hour.
#define DEFAULT_SENSOR_READING_TIME_DELTA_SECONDS 60 // one minute
#define DEFAULT_IRRIGATIONS_REFRESH_TIME 3600 // one hour
#define MAX_NUMBER_OF_READING_BEFORE_REPORTING 40

// Logger
#define LOGGER_FILENAME "/home/supox/Desktop/log.txt"
#define LOGGER_DUMP_MODE_DEFAULT true

// URLS and server
#define SERVER_URL "/"
#define TIME_URL SERVER_URL "time"
#define SPRINKLER_URL SERVER_URL "sprinklers/1/"
#define SPRINKLER_CONFIGURATION_URL SPRINKLER_URL  "configuration/"
#define SENSORS_CONFIGURATION_URL SPRINKLER_URL "sensors/"
#define SPRINKLER_VALVES_URL SPRINKLER_URL "valves/"
#define SPRINKLER_IRRIGATION_URL SPRINKLER_URL
#define SENSOR_URL_FORMAT SPRINKLER_URL "sensors/%d/create_reading/"
#define SENSOR_READING_JSON_FORMAT "{\"sensor_reading\":{\"sensor_value\":%.0lf,\"read_time\":%d}}"
#define ACK_STRING "{\"result\": \"OK\"}"

#define NUMBER_OF_BUTTONS 2
