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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libutils/dump.h"

#include "libutils/sock.h"
#include "libredfish/msg_reg.h"
#include "libredfish/rf_types.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libinit/libinit.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libsecurec/safe_lib.h"


/* generated from net-snmp-config */
#include <net-snmp/net-snmp-config.h>
#ifdef HAVE_SIGNAL
#include <signal.h>
#endif /* HAVE_SIGNAL */
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "ssa_memdb.h"
#include "handler.h"
const char *app_name = "rmm_notify";
static int reconfig = 0;

extern int netsnmp_running;

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

RETSIGTYPE
stop_server(UNUSED int a) {
    netsnmp_running = 0;
}

#ifdef SIGHUP
RETSIGTYPE
hup_handler(int sig)
{
    reconfig = 1;
    signal(SIGHUP, hup_handler);
}
#endif

union IP_TYPE{
    char c[4]; 
    long int src;
};

static void set_socket_addr(struct sockaddr_in *addr, int port)
{
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

static int snmp_msg_handler(struct rf_log_req_info *req_info)
{
    char msg_id_str[256] = {0};
    struct dest_info *listener;
    struct dest_info *tmp;
    struct dest_info *tmp2;
    char msg[256] = {0};

    int msg_sn = req_info->data.fmt1.msg_sn;
    int level = req_info->data.fmt1.level;

    msg_reg_get_msg_str(msg, msg_sn, req_info->data.fmt1.str_args);

    send_notification(msg_sn, msg);
    return 0;
}

static int snmp_trap_ip_handler(int is_remove, char* ip, int port)
{
    char dest_port[5] = {0};
    snprintf_s_i(dest_port, sizeof(dest_port), "%d", port);
    if(is_remove) {
        ssa_memdb_remove_dest_url(ip, dest_port);
    } else {
        ssa_memdb_add_dest_url(ip, dest_port);
    }
    return 0;
}

int main (int argc, char **argv)
{
    int arg;
    char* cp = NULL;
    int port  = 0;
    int fd = -1;
    int addrlen  = 0;
    fd_set fds;
    int rc = 0;
    struct sockaddr_in addr_from;
    struct rf_log_req_info req_info;
      /* we are a subagent */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                             NETSNMP_DS_AGENT_ROLE, 1);


    if (rmm_modules_init(MODULEINIT_LOG | MODULEINIT_COREDUMP,
		IPMIJSONRPC_SNMP_SUBAGENT_PORT,
		JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI)==-1) {
		exit(-1);
	}
    
    /* initialize tcpip, if necessary */
     SOCK_STARTUP;

    /* initialize the agent library */
    init_agent(app_name);
    /* initialize your mib code here */
    //init_notification();

    /* mysubagent will be used to read mysubagent.conf files. */
    init_snmp("rmm_notify");
    
    rmm_log(INFO, "%s", "snmp_subagentd daemon is Running ...\n");

    /*rmm log and redfish log init.*/
    rmm_log_init();

    port = rmm_cfg_get_port(SNMPAGENTD_PORT);
    if (port == 0) {
      rmm_log(ERROR, "%s", "Fail to call rmm_cfg_get_snmp_subagentd_port.\n");
      exit(-1);
    }
    /*Prepare to listening information.*/

    fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
    if (fd < 0)
      return -1;

    /*Redfish MessageRegistry init, load default config file.*/
    msg_reg_init(NULL);
    set_socket_addr(&addr_from, port);
    addrlen = sizeof(addr_from);

    /* In case we received a request to stop (kill -TERM or kill -INT) */
    netsnmp_running = 1;
#ifdef SIGTERM
    signal(SIGTERM, stop_server);
#endif
#ifdef SIGINT
    signal(SIGINT, stop_server);
#endif
#ifdef SIGHUP
    signal(SIGHUP, hup_handler);
#endif
    ssa_memdb_event_node_init();
    /* main loop here... */
    while(netsnmp_running) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        rc = select(fd + 1, &fds, NULL, NULL, NULL);
        if (rc < 0)
            continue;

        rc = recvfrom(fd, &req_info, sizeof(struct rf_log_req_info), 0, (struct sockaddr *)&addr_from, (socklen_t *)&addrlen);
        if (rc <= 0)
            continue;
        switch (req_info.type) {
        case RF_MSG_REQ:
            snmp_msg_handler(&req_info);
            break;
        case RF_SET_TRAP_IP:
            snmp_trap_ip_handler(req_info.data.fmt3.is_remove, req_info.data.fmt3.ip, req_info.data.fmt3.port);
            break;
        default:
            rmm_log(ERROR, "Error message type: %d", req_info.type);
            break;
        }
      
    }
    /* at shutdown time */
    snmp_shutdown(app_name);

    /* deinitialize your mib code here */

    /* shutdown the agent library */
    shutdown_agent();
    SOCK_CLEANUP;
    exit(0);
}
