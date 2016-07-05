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


#ifndef __RFMEMDB_H__
#define __RFMEMDB_H__

#include "libutils/types.h"

#define MSG_ID_KEY			"MsgID"
#define MSG_ID_STR_LEN		256

/** The destination url linked list
 */
struct dest_info {
	int8 dest[256];				//!< dest url
	int8 context[256];
	int8 name[64];
	int8 description[128];
	struct dest_info *pnext;
};

typedef struct rf_node_msg_id_map {
	int8				msg_id_str[MSG_ID_STR_LEN];		//!< msg id of redfish.
	memdb_integer		node_id;						//!< node id of the msg.
} rf_node_msg_id_map_t;

/**
* @brief Initialize redfish event node in memdb.
*
* If the event node exit, load event node info to cache.
*/
extern void rf_memdb_event_node_init(void);

/**
 * @brief get destination urls by msg id
 *
 * @param  id 			msg id of the redfish.
 * @param  location_idx	location idx of event source collection
 *						(such as pus idx of psu collection.)
 *
 * @return 0			success.
 * 		   -1			fail
 *
 */
extern int32 rf_memdb_get_listeners(int8 *id, struct dest_info **lds_header);

#endif

