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
#include <stdio.h>
#include "libcfg/cfg.h"

void pirint_rmm_log_module(int index, struct rmm_log_module log_module)
{
	printf("[%d] module name:%-16s, module level: %d, max line: %d.\n",
		index, log_module.name, log_module.level, log_module.max_lines);
}

int main(int argc, char **argv)
{
	char username[16] = {0};
	char password[16] = {0};
	char tty_infos[MAX_TTY_INFOS_CNT][RMM_TTY_INFO_LEN];
	int tty_info_cnt = 0;
	int index = 0;

	struct rmm_log_module modules[MAX_LOG_MODULES_CNT];
	int modules_cnt = 0;

	printf("------------------RMCP User Info-------------------\n");
	rmm_cfg_get_rmcp_username(username, 16);
	rmm_cfg_get_rmcp_password(password, 16);
	printf("username: 	%s\n", username);
	printf("password: 	%s\n", password);

	printf("\n");
	printf("------------------TTY Info-------------------\n");
	rmm_cfg_get_tty_infos((char *)tty_infos, &tty_info_cnt);
	for (index = 0; index < tty_info_cnt; index++) {
		printf("tty info [%d]: %s \n", index, tty_infos[index]);
	}

	printf("\n");
	printf("-----------------Log module Info-------------------\n");
	rmm_cfg_get_log_modules(modules, &modules_cnt);

	for (index = 0; index < modules_cnt; index++) {
		pirint_rmm_log_module(index, modules[index]);
	}

	printf("\n");
	printf("-----------------Port Info-------------------\n");
	printf("rmm_cfg_get_restd_port:				%d \n", rmm_cfg_get_port(ASSETD_PORT));
	printf("rmm_cfg_get_memdbd_port:			%d \n", rmm_cfg_get_port(MEMDBD_PORT));
	printf("rmm_cfg_get_ipmi_module_port:		%d \n", rmm_cfg_get_port(IPMIMODULE_PORT));
	printf("rmm_cfg_get_redfishd_port:			%d \n", rmm_cfg_get_port(REDFISHD_PORT));
	printf("rmm_cfg_get_upgraded_port:			%d \n", rmm_cfg_get_port(UPGRADED_PORT));
	printf("rmm_cfg_get_logd_port:				%d \n", rmm_cfg_get_port(LOGD_PORT));
	printf("rmm_cfg_get_db_syncd_port:			%d \n", rmm_cfg_get_port(DBSYNCD_PORT));
	printf("rmm_cfg_get_rmm_keeperd_port:		%d \n", rmm_cfg_get_port(RMMKEEPERD_PORT));
	printf("rmm_cfg_get_sync_db_ver_port:		%d \n", rmm_cfg_get_port(SYNCDB_VER_PORT));
	printf("rmm_cfg_get_keeper_ddb_port:		%d \n", rmm_cfg_get_port(KEEPERDB_PORT));
	printf("rmm_cfg_get_snapshot_sync_port:		%d \n", rmm_cfg_get_port(SNAPSHOT_SYNC_PORT));

	printf("rmm_cfg_get_ipmi_rmcp_client_port:	%d \n", rmm_cfg_get_port(IPMIRMCPCLIENT_PORT));
	printf("rmm_cfg_get_ipmi_ipmb_port:			%d \n", rmm_cfg_get_port(IPMIIPMB_PORT));
	printf("rmm_cfg_get_ipmi_i2cd_port:			%d \n", rmm_cfg_get_port(IPMII2CD_PORT));
	printf("rmm_cfg_get_ipmi_listen_port:		%d \n", rmm_cfg_get_port(IPMI_LISTEN_PORT));
	printf("rmm_cfg_get_ipmi_cooling_ctrl_port:	%d \n", rmm_cfg_get_port(IPMICOOLINGCTRL_PORT));
	printf("rmm_cfg_get_ipmi_power_mgmt_port:	%d \n", rmm_cfg_get_port(IPMIPOWERMGMT_PORT));
	printf("rmm_cfg_get_ipmi_asset_module_port:	%d \n", rmm_cfg_get_port(IPMIASSETMODULE_PORT));
	printf("rmm_cfg_get_ipmi_test_port:			%d \n", rmm_cfg_get_port(IPMITEST_PORT));
	printf("rmm_cfg_get_ipmi_uart_tgt_cfg_port:	%d \n", rmm_cfg_get_port(IPMIUARTTGTCFG_PORT));
	printf("rmm_cfg_get_ipmi_web_socket_port:	%d \n", rmm_cfg_get_port(IPMIWEBSOCKET_PORT));
	printf("rmm_cfg_get_ipmi_json_rpc_server_port:%d \n", rmm_cfg_get_port(IPMIJSONRPC_SERVER_PORT));
	printf("rmm_cfg_get_ipmi_json_ipc_client_port:%d \n", rmm_cfg_get_port(IPMIJSONRPC_IPMID_PORT));


	printf("\n");
	printf("-----------------Serial Info-------------------\n");
	unsigned char ch;
	printf("rmm_cfg_get_baudrate:	%d \n", rmm_cfg_get_baudrate());
	ch = rmm_cfg_get_databits();
	printf("rmm_cfg_get_databits:	%c \n", ch);
	ch = rmm_cfg_get_stopbits();
	printf("rmm_cfg_get_stopbits:	%c \n", ch);
	ch = rmm_cfg_get_parity();
	printf("rmm_cfg_get_parity:	%c \n", ch);
	ch = rmm_cfg_get_flow_control();
	printf("rmm_cfg_get_flow_control:%c \n", ch);


	printf("\n");
	printf("-----------------vm password-------------------\n");
	rmm_cfg_get_vm_root_password(password, MAX_VM_PASSWORD_LEN, 1);
	printf("vm 1 password:	%s \n", password);
	rmm_cfg_get_vm_root_password(password, MAX_VM_PASSWORD_LEN, 2);
	printf("vm 2 password:	%s \n", password);
	rmm_cfg_get_vm_root_password(password, MAX_VM_PASSWORD_LEN, 3);
	printf("vm 3 password:	%s \n", password);
	return 0;	
}