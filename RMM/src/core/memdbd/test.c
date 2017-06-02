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


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libmemdb/memdb.h"
#include "libutils/rmm.h"

#define EVENT_NOTIFY_BY_SELECT

static void event_callback_fn(struct event_info *evt, void *cb_data)
{
	printf("event_callback_fn called!\n\n");
	if (evt->event == EVENT_NODE_CREATE || evt->event == EVENT_NODE_DELETE) {
		printf("Node %lld %s:\n"
				"    #Parent=%lld,\n"
				"    #Type  =%lld\n",
				evt->nnodeid,
				evt->event == EVENT_NODE_CREATE ? "CREATE" : "DELETE",
				evt->nparent, evt->ntype);
	} else if (evt->event == EVENT_NODE_ATTR) {
		char *action;

		if (evt->aaction == EVENT_ATTR_ACTION_ADD)
			action = "add";
		else if (evt->aaction == EVENT_ATTR_ACTION_DEL)
			action = "del";
		else if (evt->aaction == EVENT_ATTR_ACTION_MOD)
			action = "mod";
		else
			action = "n/a";

		char *data = (char *)event_attr_data(evt);
		int i = 0;

		printf("ATTR @0x%lld %s:\n"
				"    #Cookie=0x%llX,\n"
				"    #name  =%s,\n"
				"    #data  =",
				evt->anodeid,
				action,
				evt->acookie,
				event_attr_name(evt));
		if (strstr(event_attr_name(evt), "hoststat") || strstr(event_attr_name(evt), "power")) {
			printf("%s", data);
		} else {
			#if 0
			for (i = 0; i < evt->adatalen; i++)
					printf("%02X ", data[i]);
			#endif
			printf("\ndata is:%s.\n", data);
		}

		printf("\n\n");
	}
}

static void print_attrs_by_node(memdb_integer node)
{
	void *attr;
	int i, offset, size;
	struct attr_info *info;

	attr = libdb_list_attrs_by_node(DB_RMM, node, &size, LOCK_ID_NULL);
	if (attr == NULL)
		return;

	foreach_attr_info(info, offset, attr, size) {
		printf("%s:\n", attr_name(info));
		printf("  #node=%lld\n", info->node);
		printf("  #cookie=0x%llX\n", info->cookie);
		printf("  #data=%s\n", (char *)attr_data(info));

		#if 0
		data = attr_data(info);
		for (i = 0; i < info->data_len; i++)
			printf("%02X ", data[i]);

		printf("\ndata str is:%s.\n", data);
		#endif

		printf("\n\n");
	}
}

static void print_attrs_by_cookie_mask(unsigned int cmask)
{
	void *attr;
	int i, offset, size;
	struct attr_info *info;

	attr = libdb_list_attrs_by_cookie(DB_RMM, cmask, &size, LOCK_ID_NULL);
	if (attr == NULL)
		return;

	foreach_attr_info(info, offset, attr, size) {
		printf("%s:\n", attr_name(info));
		printf("  #node=%lld\n", info->node);
		printf("  #cookie=0x%llX\n", info->cookie);
		printf("  #data=%s\n", (char *)attr_data(info));

		#if 0
		data = attr_data(info);
		for (i = 0; i < info->data_len; i++)
			printf("%02X ", data[i]);

		printf("\ndata str is:%s.\n", data);
		#endif

		printf("\n\n");
	}
}

static void print_rmc_nodes(void)
{
	int i;
	int nodenum;
	struct node_info *nodeinfo;

	nodeinfo = libdb_list_node(DB_RMM, &nodenum, NULL, LOCK_ID_NULL);
	for (i = 0; i < nodenum; i++)
		printf("    %d: parent=%lld, node_id=%lld, type=%lld\n", i,
				nodeinfo[i].parent, nodeinfo[i].node_id, nodeinfo[i].type);
	libdb_free_node(nodeinfo);

	nodeinfo = libdb_list_node_by_type(DB_RMM, 2, 3, &nodenum, NULL, LOCK_ID_NULL);
	printf("\n\nList node by type [2, 3]:\n");
	for (i = 0; i < nodenum; i++)
		printf("    %d: parent=%lld, node_id=%lld, type=%lld\n", i,
			nodeinfo[i].parent, nodeinfo[i].node_id, nodeinfo[i].type);

	libdb_free_node(nodeinfo);
}

#if 0

unsigned int g_iLoop = 0;
static int memdb_chassis_pres_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer *mpb = (memdb_integer *)cb_data;

	if (NULL == cb_data) {
		return -1;
	}

	libdb_attr_set_char(DB_RMM, *mpb, AMON_CHASSIS_PRESENT_STR,
						0x0, rsp[1], 0, LOCK_ID_NULL);
	g_iLoop++;

	return 0;
}

static int memdb_fan_pres_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer *mpb = (memdb_integer *)cb_data;

	if (NULL == cb_data) {
		return -1;
	}

	libdb_attr_set_char(DB_RMM, *mpb, AMON_FAN_PRESENT_STR,
						0x0, rsp[1], 0, LOCK_ID_NULL);
	g_iLoop++;

	return 0;

}

static int memdb_psu_pres_resp_handler(unsigned char *rsp, int rsp_len,
			void *cb_data)
{
	memdb_integer *mpb = (memdb_integer *)cb_data;

	if (NULL == cb_data) {
		return -1;
	}

	libdb_attr_set_char(DB_RMM, *mpb, AMON_PSU_PRESENT_STR,
						0x0, rsp[1], 0, LOCK_ID_NULL);
	g_iLoop++;

	return 0;

}

static void memdb_request_chassis_pres(unsigned int host, memdb_integer *mpb)
{
	struct ipmi_msg req = {0};

	req.netfn = IPMI_CM_NETFN;
	req.cmd   = AMON_CHASSIS_PRESENCE_CMD;
	req.data_len = 0;

	libipmi_rmcp_cmd(host, &req, memdb_chassis_pres_resp_handler, mpb);
}

static void memdb_request_fan_pres(unsigned int host, memdb_integer *mpb)
{
	struct ipmi_msg req = {0};

	req.netfn = IPMI_CM_NETFN;
	req.cmd   = FAN_PRESENCE_CMD;
	req.data_len = 0;

	libipmi_rmcp_cmd(host, &req, memdb_fan_pres_resp_handler, mpb);
}

static void memdb_request_psu_pres(unsigned int host, memdb_integer *mpb)
{
	struct ipmi_msg req = {0};

	req.netfn = IPMI_CM_NETFN;
	req.cmd   = AMON_PSU_PRESENCE_CMD;
	req.data_len = 0;

	libipmi_rmcp_cmd(host, &req, memdb_psu_pres_resp_handler, mpb);
}

#endif

int main(int argc, char **argv)
{
#ifdef EVENT_NOTIFY_BY_SELECT
	int max_fd;
	fd_set readset;
	int rc;
	fd_set rfds;
#endif
	int i;
	int subnode_num;
	memdb_integer bmc;
	memdb_integer tmc;
	memdb_integer psu;
	memdb_integer mbp1, mbp2, mbp3;
	char pwname[128] = {0};
	memdb_integer sub_create_handle;
	memdb_integer sub_delete_handle;
	memdb_integer sub_attr_handle_pre;
	memdb_integer sub_attr_handle_sp1;
	memdb_integer sub_attr_handle_sp2;
	struct node_info *subnode;

	libdb_init();

#ifdef EVENT_NOTIFY_BY_SELECT
	if (libdb_init_subscription(NOTIFY_BY_SELECT, event_callback_fn, NULL) < 0) {
		printf("failed to init select subscirbe mode!\n");
		return -1;
	}
#else
	if (libdb_init_subscription(NOTIFY_BY_SIGNAL, event_callback_fn, NULL) < 0) {
		printf("failed to init signal subscirbe mode!\n");
		return -1;
	}
#endif

	sub_create_handle = libdb_subscribe_node_create(DB_RMM, LOCK_ID_NULL);
	sub_delete_handle = libdb_subscribe_node_delete(DB_RMM, LOCK_ID_NULL);
	/*sub_attr_handle_pre = libdb_subscribe_node_attr_all();*/
	mbp1 = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM,
							 SNAPSHOT_NEED, LOCK_ID_NULL);
	tmc = libdb_create_node(DB_RMM, mbp1, MC_TYPE_DRAWER,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	bmc = libdb_create_node(DB_RMM, tmc, MC_TYPE_BMC,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_int(DB_RMM, bmc, "ipv4_addr", 0x0,
					   (int)0x44444444, 0, LOCK_ID_NULL);

	sub_attr_handle_sp1 = libdb_subscribe_attr_by_node(DB_RMM, mbp1, LOCK_ID_NULL);

	mbp2 = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM,
							 SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, mbp2, "chassis_pres", 0x0, 0x02, 0, LOCK_ID_NULL);
	tmc = libdb_create_node(DB_RMM, mbp2, MC_TYPE_DRAWER,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, tmc, "inlet_temp", 0x0, 0x21, 0, LOCK_ID_NULL);
	tmc = libdb_create_node(DB_RMM, mbp2, MC_TYPE_DRAWER,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, tmc, "inlet_temp", 0x0, 0x22, 0, LOCK_ID_NULL);

	char tmp_c = 0;

	libdb_attr_get_char(DB_RMM, tmc, "inlet_temp", &tmp_c, LOCK_ID_NULL);

	libdb_attr_set_char(DB_RMM, tmc, "outlet_temp",	 0x1, 0x0b, 0, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, tmc, "air_flow", 0x1, 0x10, 0, LOCK_ID_NULL);
	bmc = libdb_create_node(DB_RMM, tmc, MC_TYPE_BMC,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_int(DB_RMM, bmc, "ipv4_addr", 0x0,
					   (int)0x33333333, 0, LOCK_ID_NULL);

	int tmp_i = 0;

	libdb_attr_get_int(DB_RMM, bmc, "ipv4_addr", &tmp_i, LOCK_ID_NULL);

	mbp3 = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM,
							 SNAPSHOT_NEED, LOCK_ID_NULL);
	sub_attr_handle_sp2 = libdb_subscribe_attr_special(DB_RMM, mbp3, "air", LOCK_ID_NULL);
	tmc = libdb_create_node(DB_RMM, mbp3, MC_TYPE_DRAWER,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, tmc, "outlet_temp", 0x1, 0x23, 0, LOCK_ID_NULL);
	tmc = libdb_create_node(DB_RMM, mbp3, MC_TYPE_DRAWER,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	libdb_attr_set_char(DB_RMM, tmc, "air_flow", 0x1, 0x30, 0, LOCK_ID_NULL);

	printf("\n\n\n tmc List attrs for node %lld\n", tmc);
	print_attrs_by_node(tmc);
	printf("\n\n\n bmc List attrs for node %lld\n", bmc);
	print_attrs_by_node(bmc);
	printf("\n\n\n mbp1 List attrs for node %lld\n", mbp1);
	print_attrs_by_node(mbp1);
	printf("\n\n\n List attrs for cookie mask 0x%08X\n", 0x01);
	print_attrs_by_cookie_mask(0x01);

	printf("\n\n");
	printf("RMC subnode:\n");
	subnode = libdb_list_subnode(DB_RMM, 0, &subnode_num, NULL, LOCK_ID_NULL);
	for (i = 0; i < subnode_num; i++) {
		printf("  %d. %lld:(%lld)\n", i + 1,
					subnode[i].node_id, subnode[i].type);
	}

	printf("\n\n");
	printf("MBP1 subnode:\n");
	subnode = libdb_list_subnode(DB_RMM, mbp1, &subnode_num, NULL, LOCK_ID_NULL);
	for (i = 0; i < subnode_num; i++) {
		printf("  %d. %lld:(%lld)\n", i + 1,
					subnode[i].node_id, subnode[i].type);
	}

	printf("\n\n");
	printf("MBP2 subnode:\n");
	subnode = libdb_list_subnode(DB_RMM, mbp2, &subnode_num, NULL, LOCK_ID_NULL);
	for (i = 0; i < subnode_num; i++) {
		printf("  %d. %lld:(%lld)\n", i + 1,
					subnode[i].node_id, subnode[i].type);
	}

	printf("\n\n");
	printf("MBP3 subnode:\n");
	subnode = libdb_list_subnode(DB_RMM, mbp3, &subnode_num, NULL, LOCK_ID_NULL);
	for (i = 0; i < subnode_num; i++) {
		printf("  %d. %lld:(%lld)\n", i + 1,
					subnode[i].node_id, subnode[i].type);
	}

	printf("\n\n");
	printf("MBP3 self:\n");
	subnode = libdb_get_node_by_node_id(DB_RMM, mbp3, LOCK_ID_NULL);
	if (subnode == NULL)
		printf("\nlibdb_get_node_by_node_id return NULL.\n");
	//printf("parent: %lld; node_id: %lld; type:%lld\n", subnode[0].parent, subnode[0].node_id, subnode[0].type);

	psu = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_PSU,
							SNAPSHOT_NEED, LOCK_ID_NULL);
	snprintf_s_i(pwname, sizeof(pwname), "hoststate-%u", 0x10101);
	libdb_attr_set_string(DB_RMM, psu, pwname, 0x02, "on", 0, LOCK_ID_NULL);
	snprintf_s_i(pwname, sizeof(pwname), "hoststate-%u", 0x10102);
	libdb_attr_set_string(DB_RMM, psu, pwname, 0x02, "off", 0, LOCK_ID_NULL);
	snprintf_s_i(pwname, sizeof(pwname), "hoststate-%u", 0x20101);
	libdb_attr_set_string(DB_RMM, psu, pwname, 0x02, "off", 0, LOCK_ID_NULL);
	libdb_attr_set_string(DB_RMM, psu, pwname, 0x02, "on", 0, LOCK_ID_NULL);

	char tmp_str[128] = {0};

	libdb_attr_get_string(DB_RMM, psu, pwname, tmp_str, 128, LOCK_ID_NULL);

	printf("\ntest for get, char:%d, int:%d, string:%s.\n", tmp_c, tmp_i, tmp_str);

	libdb_attr_remove(DB_RMM, psu, pwname, LOCK_ID_NULL);
	libdb_attr_set_string(DB_RMM, psu, "powertest-213", 0x02, "on", 0, LOCK_ID_NULL);
	sub_attr_handle_pre = libdb_subscribe_attr_by_prefix(DB_RMM, psu, "hoststate-", LOCK_ID_NULL);

	#if 0
	if (libipmi_init(IPMI_MEMDB_TEST_PORT) < 0)
		return -1;

	//memdb_request_chassis_pres(inet_addr("192.168.1.200"), &mbp1);
	//memdb_request_fan_pres(inet_addr("192.168.1.200"), &mbp1);
	//memdb_request_psu_pres(inet_addr("192.168.1.200"), &mbp1);
	int g_iLoop = 0;
	for (g_iLoop = 0; g_iLoop < 3;) {
		max_fd = -1;
		FD_ZERO(&rfds);

		libipmi_callback_selectfds(&rfds, &max_fd);

		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0)
			continue;

		libipmi_callback_processfds(&rfds);

	}
	#endif

	libdb_dump_nodes(DB_RMM, LOCK_ID_NULL);
	print_rmc_nodes();

	printf("\n\nDestroy all nodes!\n");
	libdb_destroy_node(DB_RMM, 0, LOCK_ID_NULL);
	libdb_dump_nodes(DB_RMM, LOCK_ID_NULL);
	print_rmc_nodes();

#if 1
#ifdef EVENT_NOTIFY_BY_SELECT
		max_fd = -1;
		FD_ZERO(&readset);
		libdb_event_selectfds(&readset, &max_fd);

		if (select(max_fd + 1, &readset, NULL, 0, NULL) > 0)
			libdb_event_processfds(&readset);
#endif

#endif
	libdb_unsubscribe_event(DB_RMM, sub_create_handle, LOCK_ID_NULL);
	libdb_unsubscribe_event(DB_RMM, sub_delete_handle, LOCK_ID_NULL);
	libdb_unsubscribe_event(DB_RMM, sub_attr_handle_pre, LOCK_ID_NULL);
	libdb_unsubscribe_event(DB_RMM, sub_attr_handle_sp1, LOCK_ID_NULL);
	libdb_unsubscribe_event(DB_RMM, sub_attr_handle_sp2, LOCK_ID_NULL);

	return 0;
};

