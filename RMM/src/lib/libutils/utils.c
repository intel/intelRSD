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


#include <uuid.h>


#include "libutils/utils.h"
#include "libutils/rmm.h"
#include "libsecurec/safe_lib.h"


bool check_str_len(const char *str, int len)
{
	if ((str == NULL) || (strnlen_s(str, RSIZE_MAX_STR) > len)) {
		return FALSE;
	} else
		return TRUE;

	return TRUE;
}

bool check_int_range(int value, int min, int max)
{
	if (((value) < (min)) || ((value) > (max))) {
		return FALSE;
	} else
		return TRUE;

	return TRUE;
}

bool is_str_num(const char *str)
{
	int i = 0;

	if (NULL == str)
		return FALSE;

	for (i = 0; i < strnlen_s(str, RSIZE_MAX_STR); i++) {
		if (!((str[i] == '+') || (str[i] == '-') || ((str[i] >= '0') && (str[i] <= '9'))))
			return FALSE;
	}
	return TRUE;
}

bool is_str_uuid(const char *str)
{
	if ((strnlen_s(str, RSIZE_MAX_STR) <= ID_STR_MAX_LEN) && (TRUE == is_str_num(str)))
		return FALSE;
	else
		return TRUE;
}


int str2int(const char *str)
{
	int i = 0, tmp = 0;

	if (str == NULL)
		return 0;

	while (str[i] != '\0') {
		if (str[i] >= '0' && str[i] <= '9')
			tmp = tmp*10 + (str[i] - '0');
		i++;
	}
	return tmp;
}

int64 str2int64(const char* str) {
    if (NULL == str) {
        return 0;
    }
    char* end;
    rsize_t len = strnlen_s(str, RSIZE_MAX_STR);
    int64 value = strtoll(str, &end, 10);
    if (ERANGE == errno || (end && len > (end - str))) {
        value = 0;
        if (ERANGE == errno) {
            errno = 0;
        }
    }
    return value;
}

static int gen_uuid(char *uuid_str, unsigned int mode)
{
	char *str;
	uuid_t *uuid;

	uuid_create(&uuid);
	uuid_make(uuid, mode);
	str = NULL;
	uuid_export(uuid, UUID_FMT_STR, (void **)&str, NULL);
	uuid_destroy(uuid);
	memcpy_s(uuid_str, UUID_MAX_LEN, str, UUID_MAX_LEN);

	return 0;
}

int gen_uuid_v1(char *uuid_str)
{
	return gen_uuid(uuid_str, UUID_MAKE_V1);
}

int gen_uuid_v4(char *uuid_str)
{
	return gen_uuid(uuid_str, UUID_MAKE_V4);
}

bool str_end_with(char *str, char *substr)
{
    int lenstr = strnlen_s(str, RSIZE_MAX_STR);
    int lensubstr = strnlen_s(substr, RSIZE_MAX_STR);
    char *p = str + lenstr - lensubstr;
    char *p2 = substr;

    if (lenstr < lensubstr)
        return false;
    while(*p!='\0')
        if(*p++ != *p2++)
            return false;
    return true;
}

int get_stunnel_ports(int *https_port)
{
#define STUNNEL_CONF_FULLPATH   "/etc/stunnel/stunnel.conf"
#define STUNNEL_CONF_MAX_LENGTH (1024)
	int rc = -1;
	FILE *fp = NULL;
	char buf[STUNNEL_CONF_MAX_LENGTH] = {};
	char *p = NULL;
	int _https_port = -1;

	fp = fopen(STUNNEL_CONF_FULLPATH, "r");
	if (fp == NULL) {
		perror("Open");
		goto out;
	}

	rc = fread(buf, STUNNEL_CONF_MAX_LENGTH - 1, 1, fp);
	if (ferror(fp)) {
		goto out;
	}
	p = strstr(buf, "accept");
	if (p) {
		rc = sscanf(p, "accept = %d\n", &_https_port);
		if (rc == 1)
			*https_port = _https_port;
	}

out:
	if (fp)
		fclose(fp);
	return 0;
}
