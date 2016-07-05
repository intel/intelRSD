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


#include <arpa/inet.h>
#include <pthread.h>

#include "ipmi.h"
#include "ipmi_log.h"

#define IPMI_NETFN_SIZE		(32)
#define IPMI_NETFN_MASK		(IPMI_NETFN_SIZE - 1)
#define IPMI_NETFN_HASH(netfn)	\
		((netfn >> 1) & IPMI_NETFN_MASK)

struct req_sub_hndl {
	struct list_head list;

	unsigned short user_port;
	unsigned char  cmd;
};

static struct list_head sub_hndl_tbl[IPMI_NETFN_SIZE];
static pthread_mutex_t  sub_tbl_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
  *  @brief find subscrible user socket port
  *
  *  @param[in] netfn IPMI net function code
  *  @param[in] cmd IPMI command
  *  @return
  */
unsigned short find_user_port(unsigned char netfn, unsigned char cmd)
{
	unsigned short port = INVALID_PORT;
	struct list_head *head;
	struct req_sub_hndl *hndl;

	head = &sub_hndl_tbl[IPMI_NETFN_HASH(netfn)];

	pthread_mutex_lock(&sub_tbl_mutex);
	list_for_each_entry(hndl, head, list) {
		if (hndl->cmd == cmd) {
			port = hndl->user_port;
			break;
		}
	}
	pthread_mutex_unlock(&sub_tbl_mutex);

	return port;
}

/**
  *  @brief handle subscribe command
  *
  *  @param[in] user_port subscrible user socket port
  *  @param[in] netfn IPMI net function code
  *  @param[in] cmd IPMI command
  *  @return
  */
void handle_cmd_subscribe(unsigned short user_port,
			unsigned char netfn, unsigned char cmd)
{
	struct list_head *head;
	struct req_sub_hndl *hndl;

	if (user_port == INVALID_PORT || find_user_port(netfn, cmd) != INVALID_PORT) {
		IPMI_LOG_DEBUG("Invalid port: %d\n", user_port);
		return;
	}

	hndl = malloc(sizeof(*hndl));
	if (hndl == NULL) {
		IPMI_LOG_ERR("No memory for cmd subscribe!\n");
		return;
	}

	hndl->user_port = user_port;
	hndl->cmd = cmd;
	head = &sub_hndl_tbl[IPMI_NETFN_HASH(netfn)];

	pthread_mutex_lock(&sub_tbl_mutex);
	list_add_tail(&hndl->list, head);
	pthread_mutex_unlock(&sub_tbl_mutex);

	IPMI_LOG_DEBUG("App (port:%u) subscribes netfn:%02X, cmd:%02X\n",
				   ntohs(user_port), netfn, cmd);
}

/**
  *  @brief init subscrible module
  *
  *  @param
  *  @return
  */
void ipmi_subscribe_init(void)
{
	int i;

	for (i = 0; i < IPMI_NETFN_SIZE; i++)
		INIT_LIST_HEAD(&sub_hndl_tbl[i]);
}

