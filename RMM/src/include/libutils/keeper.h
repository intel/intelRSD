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


#ifndef __LIBUTILS_KEEPER_H__
#define __LIBUTILS_KEEPER_H__

enum {
	ROLE_NONE = 0,
	ROLE_LEADER,
	ROLE_FOLLOWER,
	ROLE_OBSERVER
};

#define MAX_FOLLOWERS_NUM	2

struct _role_param {
	unsigned long long node_id;
	unsigned int addr;
};

struct _keeper_state {
	unsigned long long node_id;
	unsigned char role;
	union {
		struct _role_param leader;
		struct _role_param follower[MAX_FOLLOWERS_NUM];
	} role_param;
	unsigned char rcode;

	unsigned char db_ver;
};

#endif
