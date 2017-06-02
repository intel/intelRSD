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


#include <limits.h>
#include <math.h>
#include <pthread.h>
#include "libutils/types.h"
#include "libutils/log.h"
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"


static int get_seqid(const char *data, int max_len)
{
	int ret = 0;
	char ch_id[256] = {0};
	int len = (max_len < 256 ? max_len : 256 - 1);
	char *ptr;
	strncpy_safe(ch_id, data, sizeof(ch_id), len);
	char* temp = strtok_r(ch_id, " ", &ptr);

	if (temp)
		ret = (int)strtol(temp, NULL, 10);
	else 
		ret = 0;

	return ret;
}

static int get_last_line(const char *file_path, char *line)
{
	FILE *fd;
	int ret;
	fd = fopen(file_path, "r"); // open file
	if (fd != NULL)
	{
		fseek(fd, 0, SEEK_SET); // make sure start from 0

		while(!feof(fd))
		{
			memset(line, 0x00, 256); // clean buffer
			ret = fscanf(fd, "%255[^\n]\n", line); // read file *prefer using fscanf
			if(ret == -1)
				continue;
		}
		printf("Last Line :: %s\n", line);
        fclose(fd);
	}
	return 0;
}

static int get_log_last_index(const char *file_path)
{
	char last_line[256] = {0};
	/*Get the last index of current log file*/
	get_last_line(file_path, last_line);
	return get_seqid(last_line, 256 - 1);
}

static char *get_severity_by_level(const int level)
{
	if(level < LEVEL_MAX)
		return log_severity[level];
	return EVT_SEVERITY_OTHER;
}

static void get_time_stamp(char* out)
{
	struct tm *p;
	struct timeval curr_ts;
	int len = 0;

	gettimeofday(&curr_ts, NULL);

	time_t timep;
	time(&timep); 
	p = localtime(&timep);
	#if 0
	snprintf_s_iiiiiil(out, MAX_PATH_SIZE, "%04d/%02d/%02d %02d:%02d:%02d.%06ld ",  p->tm_year + 1900,
													p->tm_mon + 1,
													p->tm_mday,
													p->tm_hour,
													p->tm_min,
													p->tm_sec,
													curr_ts.tv_usec);
	#endif

	len = snprintf_s_iiii(out, MAX_PATH_SIZE, "%04d/%02d/%02d %02d",  p->tm_year + 1900,
													p->tm_mon + 1,
													p->tm_mday,
													p->tm_hour);
	len+= snprintf_s_ii(out+len, MAX_PATH_SIZE-len, ":%02d:%02d", p->tm_min, p->tm_sec);
	len+= snprintf_s_l(out+len, MAX_PATH_SIZE-len, ".%06ld ", curr_ts.tv_usec);
	return;
}

static void log_init_handler(struct loginfo *phandler)
{
	char path[128];

	assert(phandler->module);
	snprintf_s_s(path, sizeof(path), "/var/log/%s", phandler->module);
	if (access(path, F_OK) != 0) {
		if (mkdir(path, 0755) == -1) {
			fprintf(stderr, "Failed to create log directory\n");
			exit(-1);
		}
	}

	phandler->fd = open(phandler->logfile_curr, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR
				  | S_IRGRP | S_IWGRP | S_IROTH);
	assert(phandler->fd != -1);

	phandler->curr_index = get_log_last_index(phandler->logfile_curr);
}

static void reversal_file(struct loginfo *phander)
{
	close(phander->fd);
	
	if (access(phander->logfile_old,F_OK) != 0) {
		rename(phander->logfile_curr, phander->logfile_old);
		phander->fd = open(phander->logfile_curr, O_WRONLY | O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR
				  | S_IRGRP | S_IWGRP | S_IROTH);
		assert(phander->fd != -1);
	}
	else {
		rename(phander->logfile_curr, phander->logfile_tmp);
		rename(phander->logfile_old, phander->logfile_curr);
		rename(phander->logfile_tmp, phander->logfile_old);
		phander->fd = open(phander->logfile_curr, O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR
				  | S_IRGRP | S_IWGRP | S_IROTH);
		assert(phander->fd != -1);
	}
}

static int get_log_from_file(int beg_line, char *path, char *data)
{
	int read_lines_count = 0;
	FILE *pfile;
	char *line = NULL;
	size_t len = 0;
	int index = 0;
	int read = 0;

	pfile = fopen(path, "r");
	if(!pfile) {
		read_lines_count = 0;
		goto expt_end;
	}
	while ((read = getline(&line, &len, pfile)) != -1) {
		if(index >= beg_line)
			memcpy_s(data + (index - beg_line) * 256, strnlen_s(line, RSIZE_MAX_STR), line, strnlen_s(line, RSIZE_MAX_STR));
		index++;
	}
	read_lines_count = index - beg_line;
	if (read_lines_count <=0)
		read_lines_count = 0;

expt_end:
	if(pfile)
		fclose(pfile);
	return read_lines_count;
}

void log_put(long handler, const int level, const char *func, const char *msg)
{
	va_list args;
	char time_stamp[MAX_PATH_SIZE];
	char *serverity;
	char buf[MAX_LOG_MSG_SIZE] = {0};
	int len = 0;

	struct loginfo *phandler = (struct loginfo *)handler;
	if(level > phandler->level)
		return;

	get_time_stamp(time_stamp);
	serverity = get_severity_by_level(level);

	phandler->curr_index++;
	len = snprintf_s_i(buf, MAX_LOG_MSG_SIZE, "%010d", phandler->curr_index);
	if (len < 0)
		return;

	len += snprintf_s_ssss(buf+len, MAX_LOG_MSG_SIZE-len, " %s[%s] %s: %s", time_stamp, serverity, (char *)func, (char *)msg);
	if (len < 0)
		return;

	len = write(phandler->fd, buf, len);
	if(len == -1)
		return;
	if (phandler->curr_index >= phandler->max_lines) {
		reversal_file(phandler);
		phandler->curr_index = 0;
	}
}

int log_get(long handler, int last_count, char *data)
{
	int beg = 0;
	int old_beg = 0;
	int old_end = 0;
	int is_in_old_file = 0;
	int read_lines_count = 0;
	int temp = 0;
	struct loginfo* phandler = (struct loginfo*)handler;
	
	if (phandler->curr_index >= last_count) {
		beg = phandler->curr_index - last_count;
	} else {
		old_end = get_log_last_index(phandler->logfile_old);
		old_beg = old_end + last_count - phandler->curr_index;
		if (old_beg < 0)
			old_beg = 0;
		is_in_old_file = 1;
	}

	if (is_in_old_file) {
		read_lines_count = get_log_from_file(old_beg, phandler->logfile_old, data);
	}

	temp = get_log_from_file(beg, phandler->logfile_curr, data + read_lines_count * 256);
	return read_lines_count + temp;
}

static void set_path_by_module_name(struct loginfo *phandler, const char *module_name)
{
	snprintf_s_s(phandler->logfile_curr, MAX_PATH_SIZE, "/var/log/%s/logfile", (char *)module_name);
	snprintf_s_s(phandler->logfile_tmp, MAX_PATH_SIZE, "/var/log/%s/logfile_tmp", (char *)module_name);
	snprintf_s_s(phandler->logfile_old, MAX_PATH_SIZE, "/var/log/%s/logfile_old", (char *)module_name);
}

long log_init(const char *module_name)
{
	struct loginfo* phandler = (struct loginfo*)malloc(sizeof(struct loginfo));
	set_path_by_module_name(phandler, module_name);
	strncpy_safe(phandler->module, module_name, MAX_PATH_SIZE, MAX_PATH_SIZE - 1);
	log_init_handler(phandler);
	return (long)phandler;
}

void log_set_level(long hander, int level)
{
	struct loginfo* phandler = (struct loginfo*)hander;
	phandler->level = level;
	printf("%s[%s]%s:log_level=%d\n", COLOR_LIGHT_CYAN, phandler->module, COLOR_NONE, phandler->level);
}

void log_set_max_lines(long handler, int max_lines)
{
	struct loginfo* phandler = (struct loginfo*)handler;
	phandler->max_lines= max_lines;
	printf("%s[%s]%s:max_num=%d\n", COLOR_LIGHT_CYAN, phandler->module, COLOR_NONE, phandler->max_lines);
}

void log_close(long handler)
{
	struct loginfo* phandler = (struct loginfo*)handler;
	free(phandler);
}
