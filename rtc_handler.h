#pragma once

typedef void (*rtc_on_alarm_func)(void);

void rtc_init(void);
unsigned int rtc_get_time(void);
void rtc_set_alarm(unsigned int ticks);
void rtc_set_time(unsigned int ticks);
void rtc_set_on_alarm_handler(rtc_on_alarm_func handler);
