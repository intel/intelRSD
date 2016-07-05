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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>


#include "libutils/list.h"
#include "libutils/rmm.h"
#include "libjipmi/libjipmi.h"
#include "libmemdb/memdb.h"
#include "libjipmi/jrpc_ipmi.h"
#include "libutils/sock.h"
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libutils/base64.h"
#include "libcfg/cfg.h"

int libjsonrpcapi_init(unsigned int init_bitmap, unsigned int jipmi_port)
{
	int rc = 0;

	/* memdb init */
	if (init_bitmap & JSONRPCINIT_MEMDB)
		libdb_init();

	/* jipmi interface init */
	if((init_bitmap & JSONRPCINIT_JIPMI) && (rc = libjipmi_init(jipmi_port)))
		return rc;

	return 0;
}


void libjsonrpcapi_callback_selectfds(fd_set *readset, int *max_fd)
{
	libjipmi_callback_selectfds(readset, max_fd);
	libdb_event_selectfds(readset,max_fd);
}

void libjsonrpcapi_callback_processfds(fd_set *rfds)
{
	libjipmi_callback_processfds(rfds);
	libdb_event_processfds(rfds);
}

