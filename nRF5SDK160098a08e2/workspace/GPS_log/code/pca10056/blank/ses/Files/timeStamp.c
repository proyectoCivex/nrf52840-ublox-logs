#include "timeStamp.h"

const unsigned char month_length[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};






TIMESTAMP timelib_make(TIMESTRUCT* timeinfo)
{
	int i;
	TIMESTAMP tstamp;

	// Compute the number of seconds since the year 1970 to the begining of
	// the given year on the structure, add to the output value
	tstamp = timeinfo->tm_year * (TIMELIB_SECS_PER_DAY * 365);
	// Add the seconds corresponding to leap years (add extra days)
	for (i = 0; i < timeinfo->tm_year; i++) {
		if (timelib_is_leap(i + 1970))
			tstamp += (TIMESTAMP) TIMELIB_SECS_PER_DAY;
	}
	// Add seconds for the months elapsed
	for (i = 1; i < timeinfo->tm_mon; i++) {
		if (i == 2 && timelib_is_leap(timeinfo->tm_year + 1970))
			tstamp += (TIMESTAMP) TIMELIB_SECS_PER_DAY * 29;
		else
			tstamp += (TIMESTAMP) TIMELIB_SECS_PER_DAY * month_length[i - 1];
	}
	// Add seconds for past days
	tstamp += (TIMESTAMP) (timeinfo->tm_mday - 1) * (TIMESTAMP) TIMELIB_SECS_PER_DAY;
	// Add seconds on this day
	tstamp += (TIMESTAMP) timeinfo->tm_hour * (TIMESTAMP) TIMELIB_SECS_PER_HOUR;
	tstamp += (TIMESTAMP) timeinfo->tm_min * (TIMESTAMP) TIMELIB_SECS_PER_MINUTE;
	tstamp += (TIMESTAMP) timeinfo->tm_sec;

	return tstamp;
}


 int timelib_is_leap(unsigned int year)
{
	// Must be divisible by 4 but not by 100, unless it is divisible by 400
	return((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}


//tiene un bug en los dias
void timelib_break(TIMESTAMP timeinput, TIMESTRUCT* timeinfo)
{
	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	unsigned long days;

	time = (uint32_t) timeinput;
	timeinfo->tm_sec = time % 60;

	time /= 60; // now it is minutes
	timeinfo->tm_min = time % 60;

	time /= 60; // now it is hours
	timeinfo->tm_hour = time % 24;

	time /= 24; // now it is days
	timeinfo->tm_wday = ((time + 4) % 7) + 1; // Sunday is day 1

	year = 0;
	days = 0;
	while ((unsigned) (days += (timelib_is_leap(1970 + year) ? 366 : 365)) <= time)
		year++;

	timeinfo->tm_year = year; // year is offset from 1970

	days -= timelib_is_leap(1970 + year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0

	days = 0;
	month = 0;
	monthLength = 0;
	for (month = 0; month < 12; month++) {
		// Check
		if (month == 1) {
			if (timelib_is_leap(1970 + year)) {
				monthLength = 29;
			} else {
				monthLength = 28;
			}
		} else {
			monthLength = month_length[month];
		}

		if (time >= monthLength) {
			time -= monthLength;
		} else {
			break;
		}
	}
	timeinfo->tm_mon = month + 1; // jan is month 1
	timeinfo->tm_mday = time + 1; // day of month
}
