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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include "libutils/utils.h"
#include "libmemdb/node.h"
#include "libmemdb/command.h"
#include "libmemdb/memdb.h"
#include "snap.h"
#include "memdb.h"
#include "memdb_log.h"
#include "libsecurec/safe_lib.h"


bool snapshot_in_progress = false;

#define MAX_WRITE_SIZE	256
#define MAX_READ_SIZE	MAX_WRITE_SIZE
#define MAX_ATTR_SIZE	1024

#define NODE_ATTR_HEAD_LEN	(sizeof(struct list_head) + \
							 sizeof(struct list_head) + \
							 sizeof(struct node *))

#define PARAM_ATTR_LEN	(sizeof(struct node_info) + \
						 sizeof(unsigned char))

/**
 *@brief:  Store the memdb for snapshoting.
 */
int memdb_node_snap(char *filename, unsigned char db_name)
{
	struct node *node = NULL;
	struct node_attr *attr = NULL;
	int cnt = -1;
	unsigned char buf[PARAM_ATTR_LEN] = {0};
	int fd = -1;
	unsigned char attr_cnt = 0;
	unsigned char *attr_data = NULL;
	int buf_rest_size = 0;
	int buf_assigned_size = 0;
	struct node_info nodeinfo = {0};
	struct list_head *which_node = &node_list;
	int offset = 0;
	char full_filename[128] = {0};
	struct stat sb;

	snapshot_in_progress = true;
	if (DB_POD == db_name)
		which_node = &pod_node_list;

	if (stat(DIR_FOR_SNAP, &sb) == -1) {
		if (errno == ENOENT)
			mkdir(DIR_FOR_SNAP, 0777);
	}

	snprintf_s_ss(full_filename, sizeof(full_filename), "%s%s", DIR_FOR_SNAP, filename);

	fd = open(full_filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (fd == -1) {
		perror("open");
		MEMDB_ERR("open file failed\n");
		return -1;
	}

	list_for_each_entry(node, which_node, list) {
		if ((node->parent == NULL) && (node->node_id != 0))
			continue;

		if (node->node_id != 0)
			nodeinfo.parent = (node->parent)->node_id;
		else
			nodeinfo.parent = 0;

		nodeinfo.node_id = node->node_id;
		nodeinfo.snapshot_flag = node->snapshot_flag;
		nodeinfo.type = node->type;

		if (SNAPSHOT_NEED_NOT == node->snapshot_flag)
			continue;

		memset(buf, 0, PARAM_ATTR_LEN);
		memcpy_s(buf, sizeof(buf), &nodeinfo, sizeof(struct node_info));

		attr_cnt = 0;
		offset = 0;
		buf_rest_size = 0;
		buf_assigned_size = 0;

		list_for_each_entry(attr, &node->attrs, group) {
			if (attr->snapshot_flag == SNAPSHOT_NEED_NOT)
				continue;

			if (attr->node == node) {
				attr_cnt++;

				if (buf_rest_size < (sizeof(struct node_attr_param)
								 + attr->namelen + attr->datalen)) {
					attr_data = (unsigned char *)realloc(attr_data, buf_assigned_size + MAX_ATTR_SIZE);
					if (NULL == attr_data) {
						MEMDB_ERR("malloc failed\n");
						goto err;
					}
					buf_rest_size += MAX_ATTR_SIZE;
					buf_assigned_size += MAX_ATTR_SIZE;
				}

				memcpy_s(attr_data + offset, sizeof(struct node_attr_param), (char *)attr + NODE_ATTR_HEAD_LEN,
					   sizeof(struct node_attr_param));
				offset += sizeof(struct node_attr_param);

				memcpy_s(attr_data + offset, attr->namelen, attr->name, attr->namelen);
				offset += attr->namelen;

				memcpy_s(attr_data + offset, attr->datalen, attr->data, attr->datalen);
				offset += attr->datalen;

				buf_rest_size -= (sizeof(struct node_attr_param)
								  + attr->namelen + attr->datalen);
			}
		}

		memcpy_s(buf + sizeof(struct node_info), sizeof(unsigned char), (unsigned char *)&attr_cnt,
			   sizeof(unsigned char));

		cnt = write(fd, buf, PARAM_ATTR_LEN);
		if (cnt != PARAM_ATTR_LEN) {
			MEMDB_ERR("write node info failed\n");
			goto err;
		}

		if (attr_cnt > 0) {
			cnt = write(fd, attr_data, offset);
			if (cnt != offset) {
				MEMDB_ERR("write node attr failed\n");
				goto err;
			}
		}

		if (NULL != attr_data) {
			free(attr_data);
			attr_data = NULL;
		}
	}

	close(fd);

	snapshot_in_progress = false;

	return cnt;

err:
	if (NULL != attr_data) {
		free(attr_data);
		attr_data = NULL;
	}

	close(fd);
	return -1;
}

/**
 * @brief: Load the snapshot file to the memdb.
 */
struct node *memdb_node_load(char *filename, unsigned char db_name)
{
	int cnt = -1;
	int fd = -1;
	unsigned char buf[PARAM_ATTR_LEN] = {0};
	unsigned char attr_data[MAX_ATTR_SIZE] = {0};
	unsigned char attr_cnt = 0;
	unsigned char *data = NULL;
	struct node *n = NULL;
	struct node_info *node_info = NULL;
	struct node_attr_param *attr = NULL;
	int i = 0;
	struct node *parent = NULL;
	char full_filename[128] = {0};
	char new_filename[128] = {0};

	if (chdir(DIR_FOR_SNAP) == 0) {
		snprintf_s_ss(full_filename, sizeof(full_filename), "%s%s", DIR_FOR_SNAP, filename);
	}

	fd = open(full_filename, O_RDONLY);
	if (fd == -1) {
		MEMDB_ERR("memdb load open snapshot file failed\n");
		return NULL;
	}

	do {
		memset(buf, 0, PARAM_ATTR_LEN);

		cnt = read(fd, buf, PARAM_ATTR_LEN);
		if (cnt == PARAM_ATTR_LEN) {
			node_info = (struct node_info *)buf;

			memcpy_s(&attr_cnt, sizeof(attr_cnt), buf + sizeof(struct node_info), sizeof(attr_cnt));

			if ((parent = find_node_by_node_id(db_name, node_info->parent)) != NULL) {
				if ((n = insert_node_with_node_id(db_name, parent,
											   node_info->node_id,
											   node_info->type,
											   SNAPSHOT_NEED, TRUE)) == NULL) {
					MEMDB_ERR("insert node failed\n");
					goto err;
				}
			} else {
				MEMDB_ERR("find node failed\n");
				goto err;
			}

			if (attr_cnt == 0)
				continue;

			for (i = 0; i < attr_cnt; i++) {
				cnt = read(fd, attr_data, sizeof(struct node_attr_param));
				if (cnt != sizeof(struct node_attr_param))
					goto err;

				attr = (struct node_attr_param *)attr_data;

				data = (unsigned char *)malloc(attr->datalen + attr->namelen);
				if (NULL == data) {
					MEMDB_ERR("malloc failed\n");
					goto err;
				}

				memset(data, 0, attr->datalen + attr->namelen);

				cnt = read(fd, data, attr->datalen + attr->namelen);
				if (cnt == (attr->datalen + attr->namelen)) {
					if (set_node_attr(DB_RMM, n, attr->cookie,
									  data, attr->namelen,
									  data + attr->namelen, attr->datalen,
									  SNAPSHOT_NEED, attr->type, TRUE) != 0) {
						free(data);
						MEMDB_ERR("set node attr failed\n");
						goto err;
					}
				}
				free(data);
			}
		} /* End of if (cnt == PARAM_ATTR_LEN) */
	} while (cnt > 0);

	close(fd);
	return n;

err:
	close(fd);
	return NULL;
}

