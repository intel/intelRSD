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


#ifndef __HTTP_H__
#define __HTTP_H__

#define _GNU_SOURCE

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define HTTPD_WORK_DIR		"/var/rmm/web"
#define HTTPD_SERVER_NAME	"RMM RESTful HTTP Server"
#define HTTPD_PROTOCOL		"HTTP/1.1"
#define HTTPD_RFC1123FMT	"%a, %d %b %Y %H:%M:%S GMT"
#define HTTPD_TIMO			1	/* snd & rcv timeout in seconds, 1s */


#include "liblog/log.h"
#include "libutils/types.h"

#define HTTPD_DEBUG(format, args...)    rmm_log(DBG, format, ##args)
#define HTTPD_ERR(format, args...)      rmm_log(ERROR, format, ##args)
#define HTTPD_INFO(format, args...)     rmm_log(INFO, format, ##args)

typedef union {
	struct sockaddr		sa;
	struct sockaddr_in	sa_in;
#ifdef USE_IPV6
	struct sockaddr_in6	sa_in6;
#endif
} usockaddr;

#define MAX_HEADER_LEN	1024
#define BUFFSIZ			8192

enum {
	M_GET = 1,
	M_PUT,
	M_PATCH,
	M_POST,
	M_DELETE,
	M_EVENT,
	M_OPTIONS,
	M_UNKNOWN
};

struct http_request {
	int32 fd;

	int32 hd_sz;  /*http header size*/
	int32 sz;		/* how many we read */
	int32 pos;	/* how many we consume */

	pthread_t tid_rest_req;
	int32 method;
	int8 *path;
	int32 path_offset;
	int8 *query;
	int32 query_offset;

	int32 is_websocket;
	int32 sec_websocket_version;
	int32 is_connection_upgrade;
	int64 content_length;
	int8 *sec_websocket_key;
	int32 sec_websocket_key_offset;
	int8 *origin;
	int32 origin_offset;
	int8 *content_type;
	int32 content_type_offset;
	int8 *host;
	int32 host_offset;

	usockaddr from;

	int8 *buff;
	uint32 json_start;
	uint32 json_end;
	int32 buff_size;
};

extern int32 hexit(int8 c);

extern int32 http_read(int32 fd, int8 *buff, int32 len);
extern int32 http_write(int32 fd, const int8 *data, int32 len);

extern void send_file(int32 sockfd, const int8 *name);
extern void send_error(int32 fd, int32 status, const int8 *title, const int8 *extra_header, const int8 *text);
extern void *rest_req_thread(void *args);
extern void build_rack_urls(void);

extern void *http_process(void *args);

#endif
