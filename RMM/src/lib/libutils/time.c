/**
 * Copyright (c)  2015-2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include "libsecurec/safe_lib.h"



#define TIMESTAMP_LENGTH	32


/**
 * @Ex: "2013-01-10T01:09:57"
 */
static int get_local_time(char *output, int *minutes_west)
{
	struct tm *tm;
	time_t timep;
	char timestamp[TIMESTAMP_LENGTH] = {0};
	int len = 0;

	if (NULL == output)
		return -1;

	time(&timep);
	tm = localtime(&timep);
	if (minutes_west) {
		/* tm_gmtoff holds seconds to east, we need minutes to west */
		*minutes_west = -tm->tm_gmtoff / 60;
	}

	//snprintf(timestamp, sizeof(timestamp), "%d-%02d-%02dT%02d:%02d:%02d",
				//(tm->tm_year + 1900), (tm->tm_mon + 1), tm->tm_mday,
				//tm->tm_hour, tm->tm_min, tm->tm_sec);

	len = snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, "%d-%02d",
				(tm->tm_year + 1900), (tm->tm_mon + 1));
	if (len < 0)
				return -1;

	len += snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, "-%02dT%02d",
				tm->tm_mday, tm->tm_hour);
	if (len < 0)
				return -1;

	len += snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, ":%02d:%02d",
				tm->tm_min, tm->tm_sec);
	if (len < 0)
				return -1;

	memcpy_s(output, TIMESTAMP_LENGTH, timestamp, TIMESTAMP_LENGTH);

	return 0;
}


/**
 * @Ex: "2013-01-10T01:09:57+08:00"
 */
#define GET_TIME 0
#define GET_TZ   1
static int get_iso8601_time(int type, char *output)
{
	char buff[TIMESTAMP_LENGTH] = {0};
	char timestamp[TIMESTAMP_LENGTH + 4] = {0};
	char time_zone[TIMESTAMP_LENGTH] = {0};
	int tz_min = 0;
	int len = 0;
	int minutes_west = 0;

	if (NULL == output)
		return -1;

	get_local_time(buff, &minutes_west);

	tz_min = abs(minutes_west);
	if (0 == minutes_west) {
		snprintf_s_s(timestamp, sizeof(timestamp), "%sZ", buff);
		snprintf_s_s(time_zone, sizeof(time_zone), "%s", "+00:00");
	} else {
		len = snprintf_s_ss(timestamp, sizeof(timestamp), "%s%s", buff,
				(minutes_west < 0) ? "+" : "-");
		if (len < 0)
			return -1;

		len += snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, "%02d:%02d", 
				(tz_min / 60), tz_min % 60);
		if (len < 0)
			return -1;

		len = snprintf_s_s(time_zone, sizeof(time_zone), "%s", 
				(minutes_west < 0) ? "+" : "-");
		if (len < 0)
			return -1;

		len += snprintf_s_ii(time_zone+len, sizeof(time_zone)-len, "%02d:%02d", 
				(tz_min / 60), tz_min % 60);
		if (len < 0)
			return -1;
	}

	if (type == GET_TIME)
		memcpy_s(output, TIMESTAMP_LENGTH + 4, timestamp, TIMESTAMP_LENGTH + 4);
	else if (type == GET_TZ)
		memcpy_s(output, TIMESTAMP_LENGTH, time_zone, TIMESTAMP_LENGTH);

	return 0;
}


/**
 * @Ex: "2013-01-10T01:09:57+08:00"
 */

int libutils_get_time(char* value)
{
	int type = GET_TIME;

	return get_iso8601_time(type, value);
}


int libutils_get_tz(char* value)
{
	int type = GET_TZ;

	return get_iso8601_time(type, value);
}

