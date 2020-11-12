#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <nrfx.h>


// Código sacado de : https://github.com/geekfactory/TimeLib
/*      TimeLib - Time management library for embedded devices
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx*/




//MACROS AND DEFINITIONS
#define TIMELIB_SECS_PER_DAY		(86400UL)
#define TIMELIB_SECS_PER_HOUR		(3600UL)
#define TIMELIB_SECS_PER_MINUTE		(60UL)
#define TIMELIB_DAYS_PER_WEEK		(7UL)
#define TIMELIB_SECS_PER_WEEK		(TIMELIB_SECS_PER_DAY * TIMELIB_DAYS_PER_WEEK)
#define TIMELIB_SECS_PER_YEAR		(TIMELIB_SECS_PER_WEEK * 52UL)
#define TIMELIB_SECS_YEAR_2K		(946684800UL)



/**
 * @brief Stores human readable time and date information
 *
 * Simplified structure to store human readable components of time /date similar
 * to the standard C structure for time information.
 */
typedef struct time_Struct {
	uint8_t tm_sec; //!< Seconds
	uint8_t tm_min; //!< Minutes
	uint8_t tm_hour; //!< Hours
	uint8_t tm_wday; //!< Day of week, sunday is day 1
	uint8_t tm_mday; //!< Day of the month
	uint8_t tm_mon; //!< Month
	uint8_t tm_year; //!< Year offset from 1970;
} TIMESTRUCT;



typedef uint32_t TIMESTAMP;


 /* This function generates the corresponding Unix timestamp for the provided
date and time information. The timestamp is an integral value representing
 the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC.

 See tm structure fore more details about each struct field.

@param timeinfo A structure containing the human readable elements of the
 date and time to convert to a UNIX timestamp.

 @return The UNIX Timestamp for the given time/date components
*/
TIMESTAMP timelib_make(TIMESTRUCT * timeinfo);


/**
 * @brief Get human readable time from Unix time
 *
* This function performs the conversion from Unix timestamp to human readable
* time components and places the information on a standard time structure.
*
* @param timeinput The timestamp to convert
* @param timeinfo Pointer to tm structure to store the resulting time
*/
void timelib_break(TIMESTAMP timeinput,  TIMESTRUCT* timeinfo);






/* EXAMPLE

      // Structure that holds human readable time information;
      TIMESTRUCT tinfo;
      // Variable to hold Unix time
      TIMESTAMP initialt;

      // Set time manually to 13:55:30 Jan 1st 2014
      tinfo.tm_year = 20;
      tinfo.tm_mon = 01;
      tinfo.tm_mday = 24;
      tinfo.tm_hour = 17;
      tinfo.tm_min = 23;
      tinfo.tm_sec = 30;

      // Convert time structure to timestamp
      initialt = timelib_make(&tinfo);

      //RECONVERT
      // Structure that holds human readable time information;
      TIMESTRUCT tNEWinfo;

     timelib_break(initialt,  &tNEWinfo);


Nota:
- La función timelib_make funciona pero creo que la timelib_break no
- No nos importa pq vamos a usar el tiempo TIMESTAMP (sumamos un segundo)
     */

#endif