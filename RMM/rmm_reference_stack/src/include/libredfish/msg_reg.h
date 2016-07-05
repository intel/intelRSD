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


#ifndef __LIBREDFISH_MSG_REG_H__
#define __LIBREDFISH_MSG_REG_H__

#include <stdio.h>
#include "libjson/json.h"
 
#define MAX_PARAM_TYPES			20
#define MAX_MSG_COUNT			20

#define M_STR_LEN				256
#define L_STR_LEN				512

#define MAX_ID_STR_SIZE			M_STR_LEN
#define MAX_IDENTITY_SIZE		M_STR_LEN
#define MAX_DESCRIPTION_SIZE	M_STR_LEN
#define MAX_MESSAGE_SIZE		L_STR_LEN
#define MAX_SEVERITY_SIZE		M_STR_LEN
#define MAX_RESOLUTION_SIZE		L_STR_LEN

#define MAX_NAME_SIZE			M_STR_LEN
#define MAX_MODIFIED_SIZE		M_STR_LEN
#define MAX_TYPE_SIZE			M_STR_LEN
#define MAX_LANGUAGE_SIZE		M_STR_LEN
#define MAX_PREFIX_SIZE			M_STR_LEN
#define MAX_VERSION_SIZE		M_STR_LEN
#define MAX_ENTITY_SIZE			M_STR_LEN

#define TYPE_MAX_SIZE			32
#define INVAILD_IDX				0xF

struct oem {
	int8		id_str[MAX_ID_STR_SIZE];
	int8		type[MAX_TYPE_SIZE];
	struct oem	*pnext;
};

enum {
	STRING,
	NUMBER
};

struct mr_message {
	int32		msg_sn;
	int32		index;
	int8		msg_sn_str[MAX_IDENTITY_SIZE];
	int8		desc[MAX_DESCRIPTION_SIZE];
	int8		msg[MAX_MESSAGE_SIZE];
	int8		severity[MAX_SEVERITY_SIZE];
	int32		num_of_args;
	int32		types[MAX_PARAM_TYPES];
	int8		resolution[MAX_RESOLUTION_SIZE];
	struct oem *oem_msg;

	struct mr_message *pnext;
};

struct message_registry {
	struct oem *oem_data;
	int8		odata_context[MAX_NAME_SIZE];
	int8		odata_id[MAX_MODIFIED_SIZE];
	int8		odata_type[MAX_TYPE_SIZE];
	int8		lang[MAX_LANGUAGE_SIZE];
	int8		prefix[MAX_PREFIX_SIZE];
	int8		ver[MAX_VERSION_SIZE];
	int8		entity[MAX_ENTITY_SIZE];
	int32			msg_cnt;
	struct mr_message *msg_header;
};

struct type_map {
	int8 src[TYPE_MAX_SIZE];
	int8 rep[TYPE_MAX_SIZE];
};

enum rf_ret_code {
	RF_SUCCESS,
	RF_NOT_FOUND,
	RF_FORMAT_ERR,
	RF_ERR,
	RF_NOT_INIT
};

/**
 * @brief Initialize redfish message register.
 *
 * @param  path 		the file path of redfish register.
 *
 * @return 0			success.
 * 		   -1			fail.
 */
extern int32	msg_reg_init(const int8 *path);

/**
 * @brief free the message registor data.
 *
 */
extern int32	msg_reg_destory(void);

/**
 * @brief get redfish message registor data instance.
 *
 */
extern struct message_registry *rf_get_msg_reg();

/**
 * @brief get standard redfish message registor, remove json key begin with "_"
 *
 */
extern json_t *rf_get_standard_msg_reg_json();

/**
 * @brief get redfish message args format.
 *
 * @param  fmt 				message args format.
 * @param  max_len 			max length of message args format.
 * @param  num 				the num of redfish message.
 */
extern int32 msg_reg_get_args_format(int8 *fmt, int32 max_len, int32 num);

/**
 * @brief get redfish message format.
 *
 * @param  fmt 				message format.
 * @param  max_len 			max length of message format.
 * @param  num 				the num of redfish message.
 */
extern int32 msg_reg_get_msg_format(int8 *fmt, int32 max_len, int32 num);

/**
 * @brief get redfish message identity.
 *
 * @param  identity 		message identity.
 * @param  max_len 			max length of message identity.
 * @param  num 				the num of redfish message.
 */
extern int32 msg_reg_get_msg_id_str(int8 *identity, int32 max_len, int32 num);

/**
 * @brief get redfish message entity.
 *
 * @param  msg 				message entity.
 * @param  max_len 			max length of message entity.
 * @param  num 				the num of redfish message.
 */
extern int32 msg_reg_get_msg_str(int8 *msg, int32 num, int8 *args);

/**
 * @brief get location index by args(it is decided by num).
 *
 * @param  num 				the num of redfish message.
 * @param  args 			args of message
 */
extern int32 msg_reg_get_location_idx(int32 num, int8 *args);

extern int32 rf_get_args_count(const int8 *buf, int8 **args);

#endif
