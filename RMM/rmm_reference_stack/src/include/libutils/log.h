/**
 * Copyright (c)  2015, Intel Corporation.
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


#ifndef __LIBUTILS_LOG_H__
#define __LIBUTILS_LOG_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "libutils/log_level.h"

#define MAX_PATH_SIZE 			128
#define MAX_LOG_MSG_SIZE 		512

#define EVT_SEVERITY_CRITICAL	"critical"
#define EVT_SEVERITY_ERROR		"error"
#define EVT_SEVERITY_WARNING	"warning"
#define EVT_SEVERITY_DBG		"debug"
#define EVT_SEVERITY_INFO		"info"
#define EVT_SEVERITY_OTHER		"other"

static char *log_severity[5] = {EVT_SEVERITY_CRITICAL,
								EVT_SEVERITY_ERROR, 
								EVT_SEVERITY_WARNING,
								EVT_SEVERITY_INFO,
								EVT_SEVERITY_DBG};

struct loginfo {
	char module[MAX_PATH_SIZE];
	char logfile_curr[MAX_PATH_SIZE];
	char logfile_tmp[MAX_PATH_SIZE];
	char logfile_old[MAX_PATH_SIZE];
	int  level;
	int  max_lines;
	int  fd;
	unsigned int curr_index;
};

long log_init(const char *module_name);
void log_set_level(long hander, int level);
void log_set_max_lines(long handler, int max_lines);
void log_put(long handler, const int level, const const char *func, const char *msg);
int log_get(long handler, int last_count, char *data);

void log_close(long handler);
#endif
