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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <errno.h>
#include "libutils/dump.h"

#include "config.h"
#include "libutils/upgrade.h"
#include "libutils/sock.h"
#include "libcfg/cfg.h"
#include "liblog/log.h"
#include "libsecurec/safe_lib.h"

#define EVENT_SERVICE_RESET		1
#define EVENT_FACTORY_DEFAULT	2
#define EVENT_UPGRADE			3

static int notify_client(int fd, struct sockaddr_in *addr, int len,
						 struct _upgrade_monitor *resp)
{
	int rc = -1;

	if ((fd < 0) || (NULL == addr) || (NULL == resp))
		return -1;

	rc = sendto(fd, (char *)resp, sizeof(struct _upgrade_monitor), 0,
				(struct sockaddr *)addr, len);
	if (rc < 0)
		perror("sendto");
	if (rc != sizeof(struct _upgrade_monitor))
		return -1;

	return 0;
}

static int update_event_info(int status)
{
	int fd = -1;
	int rc = -1;
	char *file_path = "/var/rmm/upgrade_monitor/";
	char *file_name = "event";
	struct stat sb;
	char full_name[128] = {0};

	if (stat(file_path, &sb) == -1) {
		if (errno == ENOENT) {
			rc = mkdir(file_path, 0777);
			if (rc < 0) {
				return -1;
			}
		}
	}

	snprintf_s_ss(full_name, sizeof(full_name), "%s%s", file_path, file_name);

	fd = open(full_name, O_RDWR | O_CREAT, S_IRWXU);
	if (fd < 0) {
		return -1;
	}

	rc = write(fd, (char *)&status, sizeof(status));
	if (rc != sizeof(status)) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

static int update_event_log(void)
{
	int fd = -1;
	int rc = -1;
	char *file_name = "/var/rmm/upgraded/event";
	struct stat sb;
	int status = 0;

	if (stat(file_name, &sb) == -1) {
		if (errno == ENOENT) {
			return 0;
		}

		return -1;
	}

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		unlink(file_name);
		return -1;
	}

	rc = read(fd, (char *)&status, sizeof(status));
	if (rc != sizeof(status)) {
		close(fd);
		unlink(file_name);
		return -1;
	}

	switch (status) {
	case EVENT_SERVICE_RESET:
		rmm_log(INFO, "Service Reset Success!\n");
		break;
	case EVENT_FACTORY_DEFAULT:
		rmm_log(INFO, "Factory Default Success!\n");
		break;
	case EVENT_UPGRADE:
		rmm_log(INFO, "Firmware Update Success!\n");
		break;
	default:
		break;
	}

	close(fd);
	unlink(file_name);

	return 0;
}

static int process_upgradation(unsigned char *name, unsigned char *path)
{
	char platform[32];
	char cmd[128] = {0};
	int status = 0;
	char *sys_path = "export PATH=/sbin/:/bin/:/usr/sbin:/usr/bin";

	if ((NULL == name) || (NULL == path))
		return -1;

	snprintf_s_s(cmd, sizeof(cmd), "%s && service rmm stop", sys_path);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return -1;
		}
	} else {
		return -1;
	}

	if (rmm_cfg_get_platform(platform, sizeof(platform)) < 0)
		strncpy_s(platform, sizeof(platform), "BDC-A", strnlen_s("BDC-A", RSIZE_MAX_STR));

	snprintf_s_ss(cmd, sizeof(cmd), "%s && echo y | apt-get remove %s", sys_path, (char *)name);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return -1;
		}
	} else {
		return -1;
	}

	snprintf_s_sss(cmd, sizeof(cmd), "%s && echo %s|dpkg -i %s/rmm*", sys_path, platform, (char *)path);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return -1;
		}
	} else {
		return -1;
	}

	update_event_info(EVENT_UPGRADE);

	snprintf_s_s(cmd, sizeof(cmd), "%s && service rmm start", sys_path);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) == 0) {
			return 0;
		}
	}

	return -1;
}

static int process_reset(unsigned char option)
{
	char cmd[128] = {0};
	int status = 0;
	char *sys_path = "export PATH=/sbin/:/bin/:/usr/sbin:/usr/bin";

	snprintf_s_s(cmd, sizeof(cmd), "%s && service rmm stop", sys_path);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			return -1;
		}
	} else {
		return -1;
	}

	if (option == SERVICE_RESET_PREPARE)
		update_event_info(EVENT_SERVICE_RESET);
	else if (option == FACTORY_DEFAULT_PREPARE)
		update_event_info(EVENT_FACTORY_DEFAULT);

	snprintf_s_s(cmd, sizeof(cmd), "%s && service rmm start", sys_path);
	status = system(cmd);
	if (status < 0) {
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) == 0) {
			return 0;
		}
	}

	return -1;
}


int create_listen_socket(int *fd)
{
	int port;

	port = rmm_cfg_get_port(UPGRADED_PORT);
	if (port == 0) {
		printf("Fail to call rmm_cfg_get_upgraded_port.\n");
		exit(-1);
	}
	*fd = create_udp_listen(INADDR_LOOPBACK, port, 0, 0);
	if (*fd < 0)
		exit(-1);

	return 0;
}

#ifdef RMM_WATCHDOG
static int is_process_alive(const char *pid_file)
{
	int retval = 0;
	FILE *pid_fp = NULL;
	char file_path[64];
	int pid = -1;

	pid_fp = fopen(pid_file, "r");
	if (pid_fp == NULL)
		goto out;
	if (fscanf(pid_fp, "%d", &pid) != 1)
		goto out;
	snprintf_s_i(file_path, sizeof(file_path), "/proc/%d", pid);
	if (access(file_path, F_OK) != 0) {
		goto out;
	}
	retval = 1;
out:
	if (pid_fp)
		fclose(pid_fp);
	return retval;
}

char *pid_files[] = {
	"/var/run/AssetModule.pid",
	"/var/run/Assetd.pid",
	"/var/run/IPMIModule.pid",
	"/var/run/Memdbd.pid",
	"/var/run/RMMLogd.pid",
	"/var/run/Redfishd.pid",
	"/var/run/Registerd.pid",
	"/var/run/Restd.pid",
	"/var/run/SNMPSubagentd.pid",
	"/var/run/snmpd.pid",
	"/var/run/stunnel.pid"
};

static int need_restart_rmm(void)
{
	int i = 0;
	int retval = 0;
	int alive_process = 0;
	int total = sizeof(pid_files)/sizeof(char *);

	for (i = 0; i < total; i++) {
		if (is_process_alive(pid_files[i]))
			alive_process++;
	}
	if (alive_process > 0 && alive_process < total)
		return 1;
	else
		return 0;
}
#endif

void main_loop(int fd)
{
	int rc = -1;
	
	fd_set fds;
	struct _upgrade_monitor req = {0};
	struct _upgrade_monitor resp = {0};
	int len = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	unsigned int current_seq = 0;
	unsigned char deb_name[MAX_DEB_NAME_LEN] = {0};
	unsigned char deb_path[MAX_DEB_PATH_LEN] = {0};
	struct timeval timeo;

#ifdef RMM_WATCHDOG
	int prev_need_restart = 0;
	int need_restart = 0;
#endif

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		timeo.tv_sec = 0;
		timeo.tv_usec = 1000*1000;

		rc = select(fd + 1, &fds, NULL, NULL, &timeo);
		if (rc < 0)
			continue;
		else if (rc == 0) {
#ifdef RMM_WATCHDOG
			need_restart = need_restart_rmm();
			if (prev_need_restart && need_restart)
				process_reset(-1);
			prev_need_restart = need_restart;
#endif
			continue;
		}

		rc = recvfrom(fd, (char *)&req, sizeof(req), 0,
					  (struct sockaddr *)&addr, (socklen_t *)&len);
		if (rc != sizeof(req))
			continue;

		if  (UPGRADE_PREPARE == req.sync) {
			resp.sync = UPGRADE_PREPARE_ACK;
			resp.seq = req.seq + 1;

			memcpy_s(deb_name, sizeof(deb_name), req.deb_name, MAX_DEB_NAME_LEN);
			memcpy_s(deb_path, sizeof(deb_path), req.deb_path, MAX_DEB_PATH_LEN);

			notify_client(fd, &addr, len, &resp);

			current_seq = resp.seq;
		} else if ((UPGRADE_CONFIRM == req.sync) && ((current_seq + 1) == req.seq)) {
			process_upgradation(deb_name, deb_path);
		} else if (SERVICE_RESET_PREPARE == req.sync) {
			process_reset(SERVICE_RESET_PREPARE);
		} else if (FACTORY_DEFAULT_PREPARE == req.sync) {
			process_reset(FACTORY_DEFAULT_PREPARE);
		} else
			continue;
	} /* End of while (1) */

	return;
}

int main(void)
{
	int fd= 0;
	
	enable_core_dump();
	sleep(2);
	rmm_log_init();
	
	update_event_log();

	create_listen_socket(&fd);

	rmm_log(INFO, "upgraded service running.....\n");

	main_loop(fd);

	return 0;
}
