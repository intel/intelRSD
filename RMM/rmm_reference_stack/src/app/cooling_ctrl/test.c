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


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libutils/rmm.h"
#include "libmemdb/memdb.h"
#include "cooling_ctrl.h"

void cooling_usage(void)
{
	printf("Cooling Usage:\n");
	printf("	-h, --help  : print this info\n");
	printf("	-p <...>	: the policy\n");
	printf("	-n <...>	: the node_id\n");
	printf("	--cm <...>	: the cm thermal level\n");
	printf("	--tmc <...>	: the tmc thermal level\n");
	printf("	--create	: create new tmc for the specfic cm\n");
	printf("	--delete	: delete the cm or tmc with the node_id\n");

	printf("\n");

	return;
}

int main(int argc, char **argv)
{
	int i;
	int subnode_num;
	struct node_info *subnode = NULL;

#if 1
	char *str_node_id = NULL;
	char *str_cm_level = NULL;
	char *str_tmc_level = NULL;
	unsigned int node_id = 0;
	char *policy = NULL;
	int cm_level = 0;
	int tmc_level = 0;

	int cur_arg = 1;
	int create_flag = 0;
	int delete_flag = 0;

	if (argc < 2) {
		cooling_usage();
		return -1;
	}

	while (cur_arg < argc) {
		if (strcmp(argv[cur_arg], "-p") == 0) {
			cur_arg++;
			if (cur_arg < argc) {
				policy = argv[cur_arg++];
			}
		} else if (strcmp(argv[cur_arg], "-n") == 0) {
			cur_arg++;
			if (cur_arg < argc) {
				str_node_id = argv[cur_arg++];
				node_id = atoi(str_node_id);
				//log_ifo("node_id is %s, %d\n", str_node_id, node_id);
			}
		} else if (strcmp(argv[cur_arg], "--cm") == 0) {
			cur_arg++;
			if (cur_arg < argc) {
				str_cm_level = argv[cur_arg++];
				cm_level = strtoul(str_cm_level, 0, 16);
				//log_ifo("cm thermal level is %s, %d\n", str_cm_level, cm_level);
			}
		} else if (strcmp(argv[cur_arg], "--tmc") == 0) {
			cur_arg++;
			if (cur_arg < argc) {
				str_tmc_level = argv[cur_arg++];
				tmc_level = strtoul(str_tmc_level, 0, 16);
				//log_ifo("tmc thermal level is %s, %d\n", str_tmc_level, tmc_level);
			}
		} else if (strcmp(argv[cur_arg], "--create") == 0) {
			cur_arg++;
			create_flag = 1;
		} else if (strcmp(argv[cur_arg], "--delete") == 0) {
			cur_arg++;
			delete_flag = 1;
		} else {
			cooling_usage();
			return -1;
		}
	}
#endif
	memdb_integer cm;
	memdb_integer tmc;

#if 1
	if (str_node_id == NULL) {
		cm = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_CM,
							   SNAPSHOT_NEED, LOCK_ID_NULL);
		if (str_cm_level != NULL)
			libdb_attr_set_int(DB_RMM, cm,
							   POLICY_AGGREGATED_THERMAL_STR, 0x0, cm_level, 0, LOCK_ID_NULL);

		tmc = libdb_create_node(DB_RMM, cm, MC_TYPE_DRAWER,
								SNAPSHOT_NEED, LOCK_ID_NULL);
		if (str_tmc_level != NULL)
			libdb_attr_set_int(DB_RMM, tmc,
							   POLICY_AGGREGATED_THERMAL_STR, 0x0, tmc_level, 0, LOCK_ID_NULL);
	} else {
		if (delete_flag){
			libdb_destroy_node(DB_RMM, node_id, LOCK_ID_NULL);
			return 0;
		}

		if (str_cm_level != NULL) {
			libdb_attr_set_int(DB_RMM, node_id,
							   POLICY_AGGREGATED_THERMAL_STR, 0x0, cm_level, 0, LOCK_ID_NULL);
		}

		if (str_tmc_level != NULL) {
			libdb_attr_set_int(DB_RMM, node_id,
							   POLICY_AGGREGATED_THERMAL_STR, 0x0, tmc_level, 0, LOCK_ID_NULL);
		}
	}
#endif

	if (policy != NULL)
		libdb_attr_set_string(DB_RMM, MC_NODE_ROOT, COOLING_POLICY,
							  0x0, policy, 0, LOCK_ID_NULL);

	subnode = libdb_list_subnode(DB_RMM, MC_NODE_ROOT, &subnode_num, NULL, LOCK_ID_NULL);
	int tmc_node_num = 0;
	//struct node_info *tmc_node = NULL;
	int j = 0;

	for (i = 0; i < subnode_num; i++) {
		//log_ifo("subnode node_id is %lu\n", subnode[i].node_id);

		if (create_flag && (str_node_id != NULL)) {
			if (subnode[i].node_id == node_id) {
				tmc = libdb_create_node(DB_RMM, node_id, MC_TYPE_DRAWER,
										SNAPSHOT_NEED, LOCK_ID_NULL);
				if (str_tmc_level != NULL) {
					libdb_attr_set_int(DB_RMM, tmc,
									   CCTRL_TMC_THERMAL_STR, 0x0, tmc_level, 0, LOCK_ID_NULL);
				}
			}
		}

		//tmc_node = libdb_list_subnode_by_type(subnode[i].node_id, MC_TYPE_TMC, &tmc_node_num);
		for (j = 0; j < tmc_node_num; j++) {
			//log_ifo("	--->tmc node node_id is %lu\n", tmc_node[j].node_id);
		}

	}

	return 0;
};
