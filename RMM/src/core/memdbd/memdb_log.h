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


#ifndef __MEMDB_LOG_H_
#define __MEMDB_LOG_H_

#include "libutils/types.h"

#define MEMDB_LOG_CREATE_NODE	1
#define MEMDB_LOG_DESTROY_NODE	2
#define MEMDB_LOG_SET_ATTR		3
#define MEMDB_LOG_REMOVE_ATTR	4

#define MEMDB_LOG_RMM_FILE			"/var/Memdb/memdb_rmm_update.log"
#define MEMDB_LOG_POD_FILE			"/var/Memdb/memdb_pod_update.log"

#define MEMDB_LOG_MAX_NUMBER		100

extern int memdb_log_get_status(void);
extern int memdb_log_get_rmm_number(void);
extern int memdb_log_get_pod_number(void);
extern void memdb_log_set_rmm_number(int var);
extern void memdb_log_set_pod_number(int var);
extern int memdb_log(memdb_integer db_name, const char *file,
					 void *param, int action);
extern int memdb_log_load(memdb_integer db_name, const char *file);

#endif
