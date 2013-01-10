#include "Alarm.h"

Alarm::Alarm(const double value, const AlarmType _type) : alarm_value(value), type(_type)
{
}

Alarm::~Alarm()
{
}

bool Alarm::will_alarm(const double sensor_value)
{
    switch(type) {
        case EQUAL :
            return alarm_value == sensor_value;
        case NOT_EQUAL :
            return alarm_value != sensor_value;
        case GREATER_THAN :
            return sensor_value > alarm_value;
        case LESS_THAN :
            return sensor_value < alarm_value;
        default:
            return false;
    }


}
