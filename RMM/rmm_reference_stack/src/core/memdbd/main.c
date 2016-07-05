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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "libutils/dump.h"

#include "snap.h"
#include "memdb.h"
#include "memdb_log.h"
#include "libmemdb/command.h"
#include "libmemdb/event.h"
#include "libmemdb/node.h"
#include "libmemdb/memdb.h"
#include "libutils/sock.h"
#include "libmemdb/memdb_jrpc.h"
#include "libcfg/cfg.h"
#include "libinit/libinit.h"
#include "libsecurec/safe_lib.h"


/**
 * @brief: Create a socket to accept the commands from the user of memdb.
 */
static void handle_signal(int sig)
{
	/* Waiting snapshot finished, before exit. */
	if ((SIGTERM == sig) || (SIGKILL == sig)) {
		MEMDB_ERR("Prepare to exit...\n");

		while (snapshot_in_progress || (0 == memdb_log_get_status()))
			usleep(10 * 1000);
	}

	exit(0);
}

static void handle_timeout(int sig)
{
	printf("\ntimeout happend!!\n");
	db_timeout();
	process_pending_cmds();
}

static sigset_t block_timer_signal(void)
{
	sigset_t newmask, oldmask;

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGALRM);

	/* block signal and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		MEMDB_ERR("%s sigprocmask failed\n", __func__);

	return oldmask;
}

static void unblock_timer_signal(sigset_t mask)
{
	if (sigprocmask(SIG_SETMASK, &mask, NULL) < 0)
		MEMDB_ERR("%s sigprocmask failed\n", __func__);
}

static int create_listen_socket(int *fd)
{
	int memdb_port;

	memdb_port = rmm_cfg_get_port(MEMDBD_PORT);
	if (memdb_port == 0) {
		MEMDB_ERR("Fail to call rmm_cfg_get_memdbd_port.\n");
		exit(-1);
	}

	*fd = create_udp_listen(INADDR_LOOPBACK, memdb_port, 0, 0);
	if (*fd < 0) {
		printf("create udp on port %d error: %s\n", memdb_port, strerror(errno));
		exit(-1);
	}

	return 0;
}

int main(int argc, char **argv)
{
	int rc;
	int fd;
	
	char *rsp_str;
	fd_set fds;
	socklen_t addrlen;
	struct sockaddr_in addr;
	struct request_pkg req;
	char cmd_string[JSONRPC_MAX_STRING_LEN] = {0};
	sigset_t mask;

	if (rmm_modules_init(MODULEINIT_COREDUMP | MODULEINIT_LOG, 0, 0))
		exit(-1);

	libdb_init();
	int_node_module();
	int_event_module();
	
	create_listen_socket(&fd);

	addrlen = sizeof(addr);

	MEMDB_INFO("Memorydb daemon is running ...\n");

	signal(SIGTERM, handle_signal);
	signal(SIGALRM , handle_timeout);

	/* Load the existed memdb first. */
	memdb_node_load(FILE_SNAP, DB_RMM);

	/* Load the latest memdb from the logfile */
	memdb_log_load(DB_RMM, MEMDB_LOG_RMM_FILE);

	rsp_str = malloc(JSONRPC_MAX_STRING_LEN);
	for (;;) {
		mask = block_timer_signal();
		process_pending_cmds();
		unblock_timer_signal(mask);

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0)
			continue;
		memset(cmd_string, 0, JSONRPC_MAX_STRING_LEN);
		rc = recvfrom(fd, cmd_string, sizeof(cmd_string), 0, (struct sockaddr *)&addr, &addrlen);

		rmm_log(INFO, "receive:  %s.\n", cmd_string);
		rc = memdb_parse_req(cmd_string, &req);		///< free req->json when the panding command have been disposed.
		if (rc) {
			if ((rc > 0) && (rc < JSON_RPC_MAX_ERR))
				rc = jrpc_create_error_rsp_string(0, JSONRPC_ID_TYPE_NULL, err_st_t[rc].err_code, err_st_t[rc].err_msg, rsp_str);

			if (rc == 0) {
				rmm_log(INFO, "send:  %s.\n", rsp_str);
				sendto(fd, rsp_str, strnlen_s(rsp_str, JSONRPC_MAX_STRING_LEN)+1, 0, (struct sockaddr *)&addr, addrlen);
			}
			continue;
		}

		mask = block_timer_signal();
		dispose_command(fd, &req, (struct sockaddr *)&addr, addrlen);
		unblock_timer_signal(mask);
		snapshot_check();
	}

	free(rsp_str);
	close(fd);

	return 0;
}
