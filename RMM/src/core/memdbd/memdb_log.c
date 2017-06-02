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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "memdb.h"
#include "memdb_log.h"
#include "libmemdb/node.h"
#include "libutils/utils.h"
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"


#define MEMDB_LOG_MAX_LINE_SIZE		1024

#define MEMDB_LOG_NAME_PREFIX		"[name:]"
#define MEMDB_LOG_DATA_PREFIX		"[data:]"

#define MEMDB_LOG_NAME_PREFIX_LEN	7
#define MEMDB_LOG_DATA_PREFIX_LEN	7

static int var_memdb_log_ok = 1;

static int memdb_log_rmm_number;
static int memdb_log_pod_number;

int memdb_log_get_status(void)
{
	return var_memdb_log_ok;
}

int memdb_log_get_rmm_number(void)
{
	return memdb_log_rmm_number;
}

int memdb_log_get_pod_number(void)
{
	return memdb_log_pod_number;
}

void memdb_log_set_rmm_number(int var)
{
	memdb_log_rmm_number = var;
}

void memdb_log_set_pod_number(int var)
{
	memdb_log_pod_number = var;
}

static int get_local_time(char *output)
{
	struct tm *tm;
	time_t timep;
	char timestamp[128] = {0};
	int len = 0;

	if (NULL == output)
		return -1;

	time(&timep);
	tm = localtime(&timep);

	//snprintf(timestamp, sizeof(timestamp), "%d-%02d-%02dT%02d:%02d:%02d",
				//(tm->tm_year + 1900), (tm->tm_mon + 1), tm->tm_mday,
				//tm->tm_hour, tm->tm_min, tm->tm_sec);

	len = snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, "%d-%02d",
				(tm->tm_year + 1900), (tm->tm_mon + 1));
	if (len < 0)
		return -1;

	len += snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, "-%02dT%02d",
				tm->tm_mday, tm->tm_hour);
	if (len < 0)
		return -1;

	len += snprintf_s_ii(timestamp+len, sizeof(timestamp)-len, ":%02d:%02d",
				tm->tm_min, tm->tm_sec);
	if (len < 0)
		return -1;

	memcpy_s(output, 128, timestamp, 128);

	return 0;
}

/**
 * @brief: when add/modify/remove node/attribute, write the operation to
 *         log file first.
 */
int memdb_log(memdb_integer db_name, const char *file,
			  void *param, int action)
{
	if (NULL == param)
		return -1;

	var_memdb_log_ok = 0;

	int fd = -1;
	char buf[MEMDB_LOG_MAX_LINE_SIZE] = {0};
	int rc = -1;
	int cnt = 0;
	char cur_time[128] = {0};
	struct node *node = NULL;
	struct node_attr *attr = NULL;

	if (DB_RMM == db_name) {
		++memdb_log_rmm_number;
	} else if (DB_POD == db_name) {
		++memdb_log_pod_number;
	} else {
		MEMDB_ERR("Unknown memdb name\n");
		var_memdb_log_ok = 1;
		return -1;
	}

	fd = open(file, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
	if (fd < 0) {
		var_memdb_log_ok = 1;
		return -1;
	}

	get_local_time(cur_time);

	switch (action) {
	case MEMDB_LOG_CREATE_NODE:
		node = (struct node *)param;
		//cnt = snprintf(buf, sizeof(buf), "%s %d %lld %lld %lld\n",
					  //cur_time, action, node->node_id, node->parent->node_id, node->type);

		cnt = snprintf_s_si(buf, sizeof(buf), "%s %d", cur_time, action);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", node->node_id);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", node->parent->node_id);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld\n", node->type);
		if (cnt < 0)
			goto err_memdb_log_fail;
		break;
	case MEMDB_LOG_DESTROY_NODE:
		node = (struct node *)param;
		//cnt = snprintf(buf, sizeof(buf), "%s %d %lld\n", cur_time, action, node->node_id);

		cnt = snprintf_s_si(buf, sizeof(buf), "%s %d", cur_time, action);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld\n", node->node_id);
		if (cnt < 0)
			goto err_memdb_log_fail;

		break;
	case MEMDB_LOG_SET_ATTR:
		attr = (struct node_attr *)param;
		//cnt = snprintf(buf, sizeof(buf), "%s %d %lld %lld %lld %lld %lld [name:]",
			//		  cur_time, action, attr->node->node_id, attr->cookie,
				//	  attr->type, attr->namelen, attr->datalen);

		cnt = snprintf_s_si(buf, sizeof(buf), "%s %d", cur_time, action);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", attr->node->node_id);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", attr->cookie);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", attr->type);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", attr->namelen);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld [name:]", attr->datalen);
		if (cnt < 0)
			goto err_memdb_log_fail;

		memcpy_s(buf + cnt, sizeof(buf)-cnt, attr->name, attr->namelen);
		cnt += attr->namelen;
		strncpy_safe(buf + cnt, "[data:]", sizeof(buf) - cnt, 7);
		cnt += 7;
		memcpy_s(buf + cnt, sizeof(buf)-cnt, attr->data, attr->datalen);
		cnt += attr->datalen;

		strncpy_safe(buf + cnt, "\n", sizeof(buf) - cnt, 1);
		cnt += 1;

		break;
	case MEMDB_LOG_REMOVE_ATTR:
		attr = (struct node_attr *)param;
		//cnt = snprintf(buf, sizeof(buf), "%s %d %lld %lld [name:]", cur_time, action, attr->node->node_id,
					 // attr->namelen);

		cnt = snprintf_s_si(buf, sizeof(buf), "%s %d", cur_time, action);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld", attr->node->node_id);
		if (cnt < 0)
			goto err_memdb_log_fail;

		cnt += snprintf_s_ll(buf+cnt, sizeof(buf)-cnt, " %lld [name:]", attr->namelen);
		if (cnt < 0)
			goto err_memdb_log_fail;

		memcpy_s(buf + cnt, sizeof(buf)-cnt, attr->name, attr->namelen);
		cnt += attr->namelen;

		strncpy_safe(buf + cnt, "\n", sizeof(buf) - cnt, 1);
		cnt += 1;
		break;
	default:
		MEMDB_ERR("memdb log Unknown action\n");
		break;
	}

	if (cnt <= 0)
		goto err;

	rc = write(fd, buf, cnt);
	if (rc <= 0)
		goto err;

	close(fd);
	var_memdb_log_ok = 1;
	return 0;

err_memdb_log_fail:
	close(fd);
	return -1;

err:
	close(fd);
	var_memdb_log_ok = 1;

	return -1;
}

/**
 * @brief: write the node and attributes read from the log file to the memdb.
 */
int memdb_log_load(memdb_integer db_name, const char *file)
{
	FILE *fp = NULL;
	int rc = -1;
	char r_buf[MEMDB_LOG_MAX_LINE_SIZE] = {0};
	int action = 0;
	memdb_integer node_id = 0;
	struct node *parent = NULL;
	struct node *node = NULL;
	struct node *n = NULL;
	memdb_integer parent_id = 0;
	memdb_integer type = 0;
	memdb_integer cookie = 0;
	unsigned char name[256] = {0};
	unsigned int name_len = 0;
	unsigned char data[256] = {0};
	unsigned int data_len = 0;
	char *search = NULL;

	fp = fopen(file, "r");
	if (NULL == fp)
		return -1;

	while (NULL != fgets(r_buf, MEMDB_LOG_MAX_LINE_SIZE, fp)) {
		rc = sscanf(r_buf, "%*s%d%lld", &action, &node_id);
		if (rc != 2)
			continue;

		switch (action) {
		case MEMDB_LOG_CREATE_NODE:
			rc = sscanf(r_buf, "%*s%*s%*s%lld%lld", &parent_id, &type);
			if (rc != 2)
				break;

			if ((parent = find_node_by_node_id(db_name, parent_id)) != NULL) {
				if (NULL == (n = insert_node_with_node_id(db_name, parent,
														  node_id, type,
														  SNAPSHOT_NEED, TRUE))) {
					MEMDB_ERR("insert node failed\n");
					break;
				}
			}
			break;
		case MEMDB_LOG_DESTROY_NODE:
			if (NULL != (node = find_node_by_node_id(db_name, node_id))) {
				destroy_node(db_name, node, TRUE);
			}
			break;
		case MEMDB_LOG_SET_ATTR:
			rc = sscanf(r_buf, "%*s%*s%*s%lld%lld%u%u", &cookie, &type,
						&name_len, &data_len);

			if (rc != 4)
				break;

			if (name_len > 256 || data_len > 256) {
				printf("invalid length");
				break;
			}

			search = strstr(r_buf, MEMDB_LOG_NAME_PREFIX);
			if (NULL == search)
				break;

			memcpy_s(name, sizeof(name), search + MEMDB_LOG_NAME_PREFIX_LEN, name_len);

			search = strstr(search + MEMDB_LOG_NAME_PREFIX_LEN + name_len,
							MEMDB_LOG_DATA_PREFIX);
			if (NULL == search)
				break;

			memcpy_s(data, sizeof(data), search + MEMDB_LOG_DATA_PREFIX_LEN, data_len);

			if (NULL != (node = find_node_by_node_id(db_name, node_id))) {
				if (set_node_attr(db_name, node, cookie,
								  name, (unsigned short)name_len,
								  data, (unsigned short)data_len,
								  SNAPSHOT_NEED, type, TRUE) != 0) {
					printf("set_node_attr failed");
				}
			}
			break;
		case MEMDB_LOG_REMOVE_ATTR:
			search = strstr(r_buf, MEMDB_LOG_NAME_PREFIX);
			if (NULL == search)
				break;

			memcpy_s(name, sizeof(name), search + MEMDB_LOG_NAME_PREFIX_LEN, name_len);

			if (NULL != (node = find_node_by_node_id(db_name, node_id))) {
				if (0 != remove_node_attr(db_name, node, name,
										  (unsigned short)name_len, TRUE))
					break;
			}
			break;
		default:
			MEMDB_ERR("Unknown action\n");
			break;
		}

	}

	fclose(fp);

	return 0;
}
