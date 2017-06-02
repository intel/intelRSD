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


#ifndef __LIBUTILS_TYPES_H__
#define __LIBUTILS_TYPES_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

typedef char                    int8;                                               // 1 byte
typedef short                   int16;                                              // 2 bytes
typedef int                     int32;                                              // 4 bytes
typedef long long               int64;                                              // 8 bytes
typedef unsigned char           uint8;                                              // 1 byte
typedef unsigned short          uint16;                                             // 2 bytes
typedef unsigned int            uint32;                                             // 4 bytes
typedef unsigned long long      uint64;                                             // 8 bytes


#ifndef __cplusplus
typedef unsigned char           bool;
#endif


#define true	1
#define false	0

#define BYTE_LEN  0x01
#define WORD_LEN  0x02


#define COLOR_NONE          "\033[m"
#define COLOR_RED           "\033[0;32;31m"
#define COLOR_LIGHT_RED     "\033[1;31m"
#define COLOR_GREEN         "\033[0;32;32m"
#define COLOR_LIGHT_GREEN   "\033[1;32m"
#define COLOR_BLUE          "\033[0;32;34m"
#define COLOR_LIGHT_BLUE    "\033[1;34m"
#define COLOR_DARY_GRAY     "\033[1;30m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_PURPLE        "\033[0;35m"
#define COLOR_LIGHT_PURPLE  "\033[1;35m"
#define COLOR_BROWN         "\033[0;33m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_LIGHT_GRAY    "\033[0;37m"
#define COLOR_WHITE         "\033[1;37m"

typedef int64  memdb_integer;
typedef char *memdb_string;
typedef double memdb_real;


#define STR_N_CAT(dest,src)	(strncat_s((dest),RSIZE_MAX_STR,(src),(strnlen_s((src,RSIZE_MAX_STR))<(sizeof((dest))-strnlen_s((dest,RSIZE_MAX_STR))-1))?strnlen_s((src,RSIZE_MAX_STR)):(sizeof((dest))-strnlen_s((dest,RSIZE_MAX_STR))-1)))
#endif
