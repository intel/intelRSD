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


#ifndef __LIBJSONRPCAPI_H__
#define __LIBJSONRPCAPI_H__

#include "libmemdb/memdb.h"
#include "libjipmi/libjipmi.h"

typedef struct _cmd_st {
	int  evt_id;
	char *cmd_name;
} cmd_map_t;


#define JSONRPCINIT_MEMDB	(1<<0)
#define JSONRPCINIT_JIPMI	(1<<1)

extern int libjsonrpcapi_init(unsigned int init_bitmap, unsigned int jipmi_port);

extern void libjsonrpcapi_callback_selectfds(fd_set *readset, int *max_fd);

extern void libjsonrpcapi_callback_processfds(fd_set *rfds);
#endif

