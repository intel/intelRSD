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


#include "ssa_memdb.h"
#include "handler.h"
#include "libmemdb/memdb.h"
#include "libutils/rack.h"
#include "liblog/log.h"
#include "libsecurec/safe_lib.h"


#define DEST_URL    "dest"
#define DEST_SNMP_SESSION_NODE   "snmp_session_node"
#define DEST_URL_LEN  64

int ssa_node_id  = -1;
struct node_id_snmp_session_map {
	memdb_integer node_id;
	void *snmp_session;
};

static memdb_integer get_ssa_node_id(void)
{
	int output = 0;
	struct node_info *subnode = NULL;
	memdb_integer node_id = 0;
	int32 subnode_num = 0;
	int32 retry = 3;

	while (retry--) {
		subnode = libdb_list_node_by_type(DB_RMM, MC_TYPE_SSA, MC_TYPE_SSA, &subnode_num, NULL, LOCK_ID_NULL);
		if (subnode)
			break;

		sleep(1);
	}

	if (subnode_num != 1 || subnode == NULL) {
		libdb_free_node(subnode);
		return 0;
	}

	node_id = subnode[0].node_id;
	libdb_free_node(subnode);

	return node_id;
}

static memdb_integer create_ssa_node(void)
{
	memdb_integer node_id = libdb_create_node(DB_RMM, MC_NODE_ROOT, MC_TYPE_SSA, SNAPSHOT_NEED, LOCK_ID_NULL);

	return node_id;
}

static void load_ssa_dest_url(memdb_integer nid) 
{
	int dest_count = 0;
	struct node_info *ssa_dest_node_info = NULL;
	int i  = 0;
	int rc = 0;
	char dest_url[DEST_URL_LEN] = {0};
	char sink[DEST_URL_LEN] = {0};
	char com[DEST_URL_LEN] = {0};
	void *snmp_seesion = NULL;
	char str_snmp_session[16] = {0};

	ssa_dest_node_info = libdb_list_subnode(DB_RMM, nid, &dest_count, NULL, LOCK_ID_NULL);
	for (i = 0; i < dest_count; i++) {
		memset(dest_url, 0, DEST_URL_LEN);
		memset(sink, 0, DEST_URL_LEN);
		memset(com, 0, DEST_URL_LEN);
		memset(str_snmp_session, 0, 16);
		libdb_attr_get_string(DB_RMM, ssa_dest_node_info[i].node_id, DEST_URL, dest_url, DEST_URL_LEN, LOCK_ID_NULL);
		sscanf(dest_url, "%63[^:],%63s", sink, com);
		snmp_seesion = add_trap_ip(sink, NULL, com);
		snprintf_s_p(str_snmp_session, sizeof(str_snmp_session), "%p", snmp_seesion);
		rc = libdb_attr_set_string(DB_RMM, ssa_dest_node_info[i].node_id,
								DEST_SNMP_SESSION_NODE, 0, str_snmp_session,
								SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
		if (rc == -1) {
			rmm_log(ERROR, "memdb set reg port fail\n");
		}
	}
}

/*
    --ssa
      ---ssa_module
        --dest:
        --trap_session
*/
void ssa_memdb_event_node_init(void)
{
	libdb_is_ready(DB_RMM, LOCK_ID_NULL, -1);
	ssa_node_id = get_ssa_node_id();

	if (ssa_node_id == 0) {
		ssa_node_id = create_ssa_node();
	}

	load_ssa_dest_url(ssa_node_id);
}

void ssa_memdb_add_dest_url(char *dest, char *port)
{
	int dest_count = 0;
	struct node_info *ssa_dest_node_info = NULL;
	int i  = 0;
	int rc = 0;
	char dest_url[DEST_URL_LEN] = {0};
	void *snmp_seesion = NULL;
	char str_snmp_session[16] = {0};
	char new_dest_url[DEST_URL_LEN] = {0};

	snprintf_s_ss(new_dest_url, sizeof(new_dest_url), "%s:%s", dest, port);

	ssa_dest_node_info = libdb_list_subnode(DB_RMM, ssa_node_id, &dest_count, NULL, LOCK_ID_NULL);
	for (i = 0; i < dest_count; i++) {
		memset(dest_url, 0, DEST_URL_LEN);

		libdb_attr_get_string(DB_RMM, ssa_dest_node_info[i].node_id, DEST_URL, dest_url, DEST_URL_LEN, LOCK_ID_NULL);
		if(strcmp(new_dest_url, dest_url) == 0)
			return;
	}

	long ssa_module = libdb_create_node(DB_RMM, ssa_node_id, MC_TYPE_SSA_MODULE, SNAPSHOT_NEED, LOCK_ID_NULL);

	snmp_seesion = add_trap_ip(dest, NULL, port);
	memset(str_snmp_session, 0, 16);
	snprintf_s_p(str_snmp_session, sizeof(str_snmp_session), "%p", snmp_seesion);
	rc = libdb_attr_set_string(DB_RMM, ssa_module, DEST_URL, 0, new_dest_url, SNAPSHOT_NEED, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set dest url fail\n");
		return;
	}

	rc = libdb_attr_set_string(DB_RMM, ssa_module, DEST_SNMP_SESSION_NODE, 0, str_snmp_session, SNAPSHOT_NEED_NOT, LOCK_ID_NULL);
	if (rc == -1) {
		rmm_log(ERROR, "memdb set dest snmp session node fail\n");
		return;
	}

}


int ssa_memdb_remove_dest_url(char *dest, char *port)
{
    int dest_count = 0;
    struct node_info *ssa_dest_node_info = NULL;
    int i  = 0;
    char dest_url[DEST_URL_LEN] = {0};
    void *snmp_seesion = NULL;
    char str_snmp_session[DEST_URL_LEN] = {0};

    char new_dest_url[DEST_URL_LEN] = {0};
    snprintf_s_ss(new_dest_url, DEST_URL_LEN, "%s:%s", dest, port);

    ssa_dest_node_info = libdb_list_subnode(DB_RMM, ssa_node_id, &dest_count, NULL, LOCK_ID_NULL);
    for (i = 0; i < dest_count; i++) {
        memset(dest_url, 0, DEST_URL_LEN);
    
        libdb_attr_get_string(DB_RMM, ssa_dest_node_info[i].node_id, DEST_URL, dest_url, DEST_URL_LEN, LOCK_ID_NULL);
        if(strcmp(new_dest_url, dest_url) == 0){
            memset(str_snmp_session, 0, DEST_URL_LEN);
            libdb_attr_get_string(DB_RMM, ssa_dest_node_info[i].node_id, DEST_SNMP_SESSION_NODE, str_snmp_session, DEST_URL_LEN, LOCK_ID_NULL);
            sscanf(str_snmp_session, "%p", &snmp_seesion);
            remove_trap_sess(snmp_seesion);
            libdb_destroy_node(DB_RMM, ssa_dest_node_info[i].node_id, LOCK_ID_NULL);
            return 0;
        }
    }
    return -1;
}
