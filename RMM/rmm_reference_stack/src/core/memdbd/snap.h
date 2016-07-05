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


#ifndef _SNAP_H_
#define _SNAP_H_

#define DIR_FOR_SNAP	"/var/Memdb/"
#define FILE_SNAP		"memdb_upgrade.snapshot"
#define FILE_POD_SNAP	"memdb_pod_upgrade.snapshot"

int memdb_node_snap(char *filename, unsigned char db_name);

struct node *memdb_node_load(char *filename, unsigned char db_name);

#endif
