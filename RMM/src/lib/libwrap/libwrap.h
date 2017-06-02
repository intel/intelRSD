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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <termios.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include "libmemdb/memdb.h"
#include "libredfish/msg_reg.h"
#include "libredfish/rf_event.h"
#include "libredfish/mr_def.h"

#include "libwrap/wrap.h"
#include "libcfg/cfg.h"
#include "libjsonrpcapi/assetd_api.h"

#include "libjsonrpcapi/assetd_socket.h"
#include "libjsonrpcapi/asset_module_socket.h"
#include "libjsonrpcapi/assetd_api.h"
#include "libjsonrpc/jsonrpc.h"
#include "libassetd/assetd_type.h"
#include "libassetd/assetd_jrpc_def.h"
#include "libutils/ip.h"
#include "libutils/rmm_json.h"
#include "libutils/types.h"
#include "libutils/string.h"
#include "libassetmodule/am_action.h"
#include "libutils/utils.h"
#include "libutils/time.h"
#include "libsecurec/safe_lib.h"


#include "am_api.h"


result_t check_module_capability(char *module, char *cap_name);
int32 json_add_multi_member(json_t *result, int32 count, const char *members);
int memdb_filter(memdb_integer node_id);
int get_present_string(memdb_integer node_id, char* output, int max_num, int max_len);
int get_db_info_num(unsigned char db_name, memdb_integer node_id, char* name);
int32 json_add_single_member(json_t *result, const char *url, const char *element);
result_t pack_odata_head(json_t *result, const struct rest_uri_param *param, const char *name, bool redfish);

void add_odata_context(json_t *result, const struct rest_uri_param *param);
void add_odata_id(json_t *result, const struct rest_uri_param *param);
void add_odata_type(json_t *result, const int8 * type);

int get_base_element(base_element_t* be, unsigned char db_name, memdb_integer node_id);
int get_asset_info(unsigned char db_name, asset_info_t* asset, memdb_integer node_id);
int32 json_add_link(json_t *result, const char *name, const char *url);
int32 pack_rack_base_info(rack_info_t *rack_info, json_t *result);
int get_present(memdb_integer zone_node_id, unsigned int *present);
int get_drawer_pres_from_cm(char *pres);
result_t gen_odata_id(const struct rest_uri_param *param, int8 *output, int len);
result_t gen_odata_type(const struct rest_uri_param *param, int8 *output, int len, const char *name, bool redfish);



