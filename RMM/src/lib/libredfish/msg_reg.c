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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "libutils/file.h"
#include "libutils/string.h"
#include "libredfish/msg_reg.h"
#include "libredfish/rf_types.h"
#include "libutils/types.h"
#include "libcfg/platform.h"
//#include "libsecurec/safe_lib.h"

#include "libredfish/messages.h"
#include "libredfish/mr_def.h"

#ifndef MR_GENFILE
#include "libredfish/mr_def.h"
#endif

#define TYPE_STR			"string"
#define TYPE_INT			"number"
#define TYPE_STR_ID			1
#define TYPE_INT_ID			2

#define MR_JSON_FILE_PATH	"/var/rmm/redfish/MR.json"

struct type_map rf_evt_params_convet_map[2] = {
	{"%integer",	"%d"},
	{"%string",		"%s"}
};

/**
 * @brief The data pointer of redfish message registry.
 *
 */
static struct message_registry *g_mr;

int32 rf_get_args_count(const int8 *buf, int8 **args)
{
	int8 tmp[256] = {0};
	const int8 *split = ",";
	int8 *p = NULL;
	int32 i = 0;
	int8 *ptr;

	strncpy_safe(tmp, buf, sizeof(tmp), sizeof(tmp) - 1);
	p = strtok_r(tmp, split, &ptr);
	while (p != NULL) {
		args[i] = strdup(p);
		p = strtok_r(NULL, split, &ptr);
		i++;
	}

	return i;
}

/*Get message data through the message format("%d,%s,%s") and args*/
static int32 parse_msg(int8 *msg, int8 *msg_fmt, int8 **args, int32 count)
{
	int8	*str;
	int8	*arg;
	int32	index = 0;
	int32	size = 0;
	int32	i = 0;

	if (msg == NULL || msg_fmt == NULL || args == NULL)
		return -1;

	for (str = msg; *msg_fmt; ++msg_fmt) {
		if (*msg_fmt != '%') {
			*str++ = *msg_fmt;
			continue;
		}
		msg_fmt++;
		switch (*msg_fmt) {
		case 's':
		case 'd': {
			arg = args[index];
			index++;
			size = strlen(arg);
			for (i = 0; i < size; ++i)
				*str++ = *arg++;
			break;
			}
		default:
			break;
		}
	}
	*str = '\0';

	return str - msg;
}

static void str_replace(int8 *target, int32 max_len, int8 *old, int8 *str_new)
{
	int8 *buffer = (int8 *)malloc(max_len);
	int8 *beg_of_substr = NULL;

	if (buffer == NULL)
		return;

	beg_of_substr = strstr(target, old);
	if (beg_of_substr == NULL) {
		free(buffer);
		return;
	}

	strncpy_safe(buffer, target, beg_of_substr - target+1, beg_of_substr - target);
	buffer[beg_of_substr-target] = '\0';

	snprintf_s_ss(buffer + (beg_of_substr - target), max_len - (beg_of_substr - target), "%s%s", str_new, beg_of_substr + strlen(old));
	target[0] = '\0';
	strncpy_safe(target, buffer, max_len, max_len - 1);
	free(buffer);

	return str_replace(target, max_len, old, str_new);
}

static int8 *get_json_str(json_t *jobj, int8 *name)
{
	json_t *jval = json_object_get(jobj, name);

	if (jval == NULL) {
		printf("No such atrribute in the json object: %s\n", name);
		return NULL;
	}

	int8 *data = json_string_value(jval);

	return data;
}

static void load_json_str_value(int8 *dest, int32 max_len, json_t *jobj, int8 *name)
{
	int8 *data = get_json_str(jobj, name);

	if (data == NULL)
		return;
	strncpy_safe(dest, data, max_len, max_len - 1);
}

/*For example: Convert string array {"number", "string", "number"} to int array [1,2,1]*/
static void param_type_parse(char*const str_types[], unsigned short length,
			     int32 *types)
{
	int32 index = 0;

	for (index = 0; index < length; index++) {
		if (strncmp(TYPE_STR, str_types[index], strlen(TYPE_STR)) == 0)
			types[index] = TYPE_STR_ID;
		else if (strncmp(TYPE_INT, str_types[index], strlen(TYPE_INT)) == 0)
			types[index] = TYPE_INT_ID;
		else
			printf("param_type_parse error happened.\n");
	}
}
#if 0
static struct oem *oem_parse(json_t *jobj)
{
	struct oem *poem = NULL;
	struct oem *poem_header = NULL;
	json_object_t *obj = json_to_object(jobj);
	json_pair_t *pair;

	for (pair = obj->next; pair != NULL; pair = pair->next) {
		if (poem == NULL) {
			poem = (struct oem *)malloc(sizeof(struct oem));
			poem_header = poem;
		} else {
			poem->pnext = (struct oem *)malloc(sizeof(struct oem));
			poem = poem->pnext;
		}
		strncpy_safe(poem->id_str, pair->name, MAX_SEVERITY_SIZE, MAX_SEVERITY_SIZE - 1);
		load_json_str_value(poem->type, MAX_TYPE_SIZE, pair->value, "Type");
	}

	return poem_header;
}
#endif
static struct mr_message *msg_parse()
{
	struct mr_message	*pmsg		= NULL;
	struct mr_message	*pmsg_header	= NULL;
	enum MessageType	sn		= 0;

	while (NULL != static_messages[sn].desc &&
	       NULL != static_messages[sn].msg &&
	       NULL != static_messages[sn].severity) {
		if (pmsg == NULL) {
			pmsg = (struct mr_message *)malloc(sizeof(struct mr_message));
			if (pmsg == NULL) {
				printf("Failed to allocated memory.\n");
				break;
			}
			memset(pmsg, 0, sizeof(struct mr_message));
			pmsg_header = pmsg;
		} else {
			pmsg->pnext = (struct mr_message *)malloc(sizeof(struct mr_message));
			if (pmsg->pnext == NULL) {
				printf("Failed to allocated memory.\n");
				break;
			}
			memset(pmsg->pnext, 0, sizeof(struct mr_message));
			pmsg = pmsg->pnext;
		}

		strncpy_safe(pmsg->msg_sn_str, static_messages[sn].name, MAX_IDENTITY_SIZE, MAX_IDENTITY_SIZE - 1);
		pmsg->msg_sn = sn;
		pmsg->index = 0;
		strncpy_safe(pmsg->desc, static_messages[sn].desc,MAX_DESCRIPTION_SIZE, MAX_DESCRIPTION_SIZE-1);
		strncpy_safe(pmsg->msg, static_messages[sn].msg, MAX_MESSAGE_SIZE, MAX_MESSAGE_SIZE-1);
		strncpy_safe(pmsg->severity, static_messages[sn].severity,MAX_SEVERITY_SIZE, MAX_SEVERITY_SIZE-1);
		pmsg->num_of_args = static_messages[sn].num_of_args;
		param_type_parse(static_messages[sn].types,
				 static_messages[sn].num_of_args, pmsg->types);

		sn++;
	}

	return pmsg_header;
}

static void msg_reg_parse(json_t *jobj, struct message_registry *mr)
{

	load_json_str_value(mr->odata_context,		MAX_NAME_SIZE, jobj, "@odata.context");
	load_json_str_value(mr->odata_id,	MAX_MODIFIED_SIZE, jobj, "@odata.id");
	load_json_str_value(mr->odata_type,		MAX_TYPE_SIZE, jobj, "@odata.type");
	load_json_str_value(mr->lang,		MAX_LANGUAGE_SIZE, jobj, "Language");
	load_json_str_value(mr->prefix,	MAX_PREFIX_SIZE, jobj, "RegistryPrefix");
	load_json_str_value(mr->ver,		MAX_VERSION_SIZE, jobj, "RegistryVersion");
	load_json_str_value(mr->entity,	MAX_ENTITY_SIZE, jobj, "OwningEntity");

	mr->msg_header = msg_parse();
}

static struct message_registry *mr_init(int8 *data) {
	struct message_registry* mr = NULL;
	json_t* obj = json_parse(data);

	if (!obj) {
		printf("Json parse error...");
		return NULL;
	}

	mr = (struct message_registry*) malloc(sizeof(struct message_registry));

	if (mr) {
		msg_reg_parse(obj, mr);
	}
	json_free(obj);
	return mr;
}

/*
 Get the message args format from message types(Int array)
 For example: [1,2,1] => ("%d,%s,%d")
*/
int32 msg_reg_get_args_format(int8 *msg_args_fmt, int32 max_len, int32 msg_sn)
{
	struct mr_message *pmsg		= NULL;
	int32	index				= 0;
	va_list args;
	int8	tmp[256]			= {0};

	if (!g_mr)
			return RF_NOT_INIT;

	pmsg = g_mr->msg_header;
	while (pmsg) {
		/*Search all all msg, find msg idx is equal.*/
		if (msg_sn == pmsg->msg_sn) {
			for (index = 0; index < pmsg->num_of_args; index++) {
				if (index != 0)
					strncat(tmp, ",", 1);

				if (pmsg->types[index] == TYPE_STR_ID)
					strncat(tmp, "%s", 2);
				else if (pmsg->types[index] == TYPE_INT_ID)
					strncat(tmp, "%d", 2);
				else
					printf("error message type: %d\n", pmsg->types[index]);
			}

			strncpy_safe(msg_args_fmt, tmp, max_len, max_len - 1);
			return RF_SUCCESS;
		}
		pmsg = pmsg->pnext;
	}

	return RF_NOT_FOUND;
}

static struct message_registry *mr_init_via_file(const int8 *file_path)
{
	int8 *buffer = NULL;
	struct message_registry *mr = NULL;

	if (file_path == NULL)
		buffer = read_file(MR_JSON_FILE_PATH);
	else
		buffer = read_file(file_path);

	if (buffer == NULL)
		return NULL;

	mr = mr_init(buffer);
	
	free(buffer);

	return mr;
}

#ifndef MR_GENFILE
int32 msg_reg_get_location_idx(int32 msg_sn, int8 *arg_data)
{
	int8 *args[10];
	int32 location_idx = INVAILD_IDX;
	int32 zone_idx = 0;
	int32 max_zone_num = 0;
	int32 count = rf_get_args_count(arg_data, args);
	int32 location_offset = -1;
	struct mr_message *pmsg = NULL;

	if (!g_mr)
		return RF_NOT_INIT;

	pmsg = g_mr->msg_header;

	while (pmsg) {
		if (msg_sn == pmsg->msg_sn) {
			location_offset = pmsg->index;
			break;
		}
		pmsg = pmsg->pnext;
	}

	switch (msg_sn) {
	case MSGPSUResourceAdded:
	case MSGPSUResourceRemoved:
	case MSGPSUStatusChange:
		max_zone_num = MAX_PSU_NUM;
		break;
	case MSGFanResourceAdded:
	case MSGFanResourceRemoved:
	case MSGFanStatusChange:
		max_zone_num = MAX_PWM_NUM;
		break;
	default:
		break;
	}

	if (max_zone_num != 0) {
		if (count > location_offset && location_offset != -1)
				zone_idx = atoi(args[location_offset]);

		if (count > location_offset && location_offset != -1)
				location_idx = atoi(args[location_offset + 1]);
		location_idx = (zone_idx - 1) * max_zone_num + location_idx;
	} else {
		if (count > location_offset && location_offset != -1)
				location_idx = atoi(args[location_offset]);
	}

	return location_idx;
}
#endif

int32 msg_reg_get_msg_id_str(int8 *identity, int32 max_len, int32 msg_sn)
{
	struct mr_message *pmsg = NULL;

	if (!g_mr)
		return RF_NOT_INIT;

	pmsg = g_mr->msg_header;

	while (pmsg) {
		if (msg_sn == pmsg->msg_sn) {
			strncat(identity, g_mr->prefix, max_len);
			strncat(identity, g_mr->ver, max_len);
			strncat(identity, pmsg->msg_sn_str, max_len);
			break;
		}
		pmsg = pmsg->pnext;
	}

	return RF_SUCCESS;
}

int32 msg_reg_init(const int8 *path)
{
	if (g_mr == NULL)
		g_mr = mr_init_via_file(path);
	if (g_mr)
		return RF_SUCCESS;
	else
		return RF_ERR;
}

int32 msg_reg_destory(void)
{
	if (g_mr) {
		free(g_mr);
		g_mr = NULL;
	}

	return RF_SUCCESS;
}

struct message_registry *rf_get_msg_reg()
{
	return g_mr;
}

static void remove_unused_json(json_t* mobj)
{
	json_pair_t *pair;
	json_object_t *obj = json_to_object(mobj);

	for (pair = obj->next; pair != NULL; pair = pair->next) {
		json_object_remove(pair->value, "_ArgPos");
	}
}

extern json_t *rf_get_standard_msg_reg_json()
{
	int8	*buffer = NULL;
	json_t  *obj = NULL;
	json_t  *mjobj = NULL;

	buffer = read_file(MR_JSON_FILE_PATH);
	if(buffer ==NULL)
		return NULL;

	obj = json_parse(buffer);
	mjobj = json_object_get(obj, "Messages");
	if (mjobj != NULL) {
		remove_unused_json(mjobj);
	} else {
		printf("rf_get_standard_msg_reg_json Failed to get Json Messages field.\n");
	}

	free(buffer);
	return obj;
}

int32 msg_reg_get_msg_format(int8 *msg_fmt, int32 len, int32 msg_sn)
{
	struct mr_message *pmsg		= NULL;
	int32 index					= 0;
	va_list args;

	if (!g_mr)
		return RF_NOT_INIT;

	pmsg = g_mr->msg_header;
	while (pmsg) {
		if (msg_sn == pmsg->msg_sn) {
			strncpy_safe(msg_fmt, pmsg->msg, len, len - 1);
			break;
		}
		pmsg = pmsg->pnext;
	}
	if (!pmsg)
		return RF_NOT_FOUND;

	for (index = 0; index < 2; index++)
		str_replace(msg_fmt, len, rf_evt_params_convet_map[index].src, rf_evt_params_convet_map[index].rep);

	return RF_SUCCESS;
}

int32 msg_reg_get_msg_str(int8 *msg, int32 msg_sn, int8 *arg_data)
{
	if (!g_mr)
		return RF_NOT_INIT;

	int32 ret = 0;
	int8 *args[10];
	int32 count = rf_get_args_count(arg_data, args);
	int8 msg_fmt[512] = {0};

	ret = msg_reg_get_msg_format(msg_fmt, sizeof(msg_fmt), msg_sn);
	if (ret)
		return ret;

	if (parse_msg(msg, msg_fmt, args, count) == -1)
		return RF_ERR;

	return RF_SUCCESS;
}
