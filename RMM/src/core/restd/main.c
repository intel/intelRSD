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


/* http://tools.ietf.org/rfc/rfc6455.txt */

#include <sys/prctl.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>


#include "http.h"
#include "rest.h"
#include "handler/handler.h"
#include "libjsonrpcapi/libjsonrpcapi.h"
#include "libjsonrpcapi/assetd_socket.h"
#include "libwrap/wrap.h"
#include "libcfg/cfg.h"
#include "libredfish/rf_event.h"
#include "libutils/dump.h"
#include "libinit/libinit.h"
 
static int32 open_listen_socket(uint16 port)
{
	int32 fd;
	int32 val;
	int32 bind_try = 0;
	struct sockaddr_in addr;
	struct linger linger_opt = { 1, 1 };	/* Linger active, timeout 1s. */

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		HTTPD_ERR("Fail to open socket: %s\n", strerror(errno));
		return -1;
	}

	fcntl(fd, F_SETFD, FD_CLOEXEC);

	val = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val)) < 0) {
		HTTPD_ERR("Fail to setsockopt SO_REUSEADDR: %s\n", strerror(errno));
		goto fail;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	for (;;) {
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			bind_try++;
			if (bind_try >= 10) {
				HTTPD_ERR("Fail to bind: %s\n", strerror(errno));
				goto fail;
			} else
				sleep(1);
		} else
			break;
	}
	setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));

	if (listen(fd, 1) < 0) {
		HTTPD_ERR("Fail at socket listen: %s\n", strerror(errno));
		goto fail;
	}

	return fd;

fail:
	close(fd);
	return -1;
}

static void *ipmi_cb_thread(void *unused)
{
	int32	rc;
	int32	max_fd;
	fd_set	rfds;

	prctl(PR_SET_NAME, "ipmi_cb_thread");
	while (1) {
		max_fd = -1;
		FD_ZERO(&rfds);
		libjipmi_callback_selectfds(&rfds, &max_fd);
		rc = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if (rc <= 0) {
			HTTPD_IPMI_PRINT("select timeout\n");
			continue;
		}

		libjipmi_callback_processfds(&rfds);
	}
	printf("ipmi_cb_thread task exit, errno:%d %s\n", errno, strerror(errno));
	return NULL;
}


static int32 main_loop()
{
	int32 		rc = 0;
	int32 		conn_fd = -1;
	int32 		listen_fd = -1;
	fd_set		rfds;
	socklen_t	addrlen = 0;
	usockaddr	usa;
	pthread_t	tid_ipmi_cb;
	struct sigaction	sa;
	struct http_request *request = NULL;
	int32 port  = 0;

	/* ingore sigpipe */
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, 0);
	
	rest_register_handlers();

	port = rmm_cfg_get_port(RESTD_PORT);
	listen_fd = open_listen_socket(port);
	if (listen_fd == -1) {
		HTTPD_ERR("Failed to open HTTP server!\n");
		return -1;
	}

	fprintf(stderr, "Mini-HTTP Server is Running ...\n");

	/* Create IPMI callback thread. */
	if (pthread_create(&tid_ipmi_cb, NULL, ipmi_cb_thread, NULL) != 0) {
		close(listen_fd);
		HTTPD_ERR("Failed to create ipmi callback thread!\n");
		return -1;
	}
	for (;;) {
			FD_ZERO(&rfds);
			FD_SET(listen_fd, &rfds);
			rc = select(listen_fd + 1, &rfds, NULL, NULL, NULL);
			if (rc <= 0)
				continue;
	
			addrlen = sizeof(usa);
			conn_fd = accept(listen_fd, &usa.sa, &addrlen);
			if (conn_fd < 0)
				continue;
			HTTPD_INFO("restd accepted a new connection: fd = %d\n", conn_fd);
	
			request = malloc(sizeof(struct http_request));
			if (NULL == request) {
				HTTPD_ERR("Malloc error: %s\n", strerror(errno));
				return -1;
			}
			memset(request, 0, sizeof(struct http_request));
			request->fd = conn_fd;
			request->from = usa;
	
			if (pthread_create(&request->tid_rest_req, NULL, http_process, (void *)request) != 0) {
				HTTPD_ERR("Failed to create rest_req_thread!\n");
				return -1;
			}
		}

	return 0;
}

static int32 is_asset_module_ready(int32 max_retry)
{
	int32 port = 0;

	while (1) {
		if (libwrap_connect_asset_module() == RESULT_OK)
			return 0;

		if (max_retry == 0)
			return -1;
		else if (max_retry > 0)
			max_retry--;

		sleep(1);
	}
	return -1;
}

int32 main(int32 argc, int8 **argv)
{
	if (rmm_modules_init(MODULEINIT_LOG | MODULEINIT_COREDUMP | MODULEINIT_ASSETD | MODULEINIT_REDFISHD, 
					IPMIWEBSOCKET_PORT, 
					JSONRPCINIT_MEMDB | JSONRPCINIT_JIPMI) == -1) {
		exit(-1);
	}

	if (is_asset_module_ready(10) == -1)
		exit(-1);

	HTTPD_INFO("Restd start ...\n");

	main_loop();

	return 0;
}

