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


#ifndef __LIBUTILS_UTILS_H__
#define __LIBUTILS_UTILS_H__


#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include "types.h"

#define ID_STR_MAX_LEN       20

#define MAX_KEYS_NUM	16
#define MAX_URI_NODES	16

#ifndef TRUE
#define TRUE	(1)
#endif
#ifndef FALSE
#define FALSE	(0)
#endif


#define FATAL(msg)	\
	do { printf(msg); exit(-1); } while (0)



extern bool check_str_len(const char *str, int len);
extern bool check_int_range(int value, int min, int max);
extern bool is_str_num(const char *str);
extern bool is_str_uuid(const char *str);
extern int str2int(const char *str);
extern bool str_end_with(char *str, char *substr);

extern int gen_uuid_v1(char *uuid);
extern int gen_uuid_v4(char *uuid);

extern int get_stunnel_ports(int *https_port);
#endif

