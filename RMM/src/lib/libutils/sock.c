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


#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "libutils/sock.h"

#define MAX_TIMEOUT_MS 0xFFFFFFFF

int create_udp_listen(unsigned int host, int port, int broadcast, int reuse)
{
	int fd;
	int rc, val;
	struct sockaddr_in addr;
	val = 1;
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd <= 0) {
		printf("socked failed: %s\n", strerror(errno));
		return -1;
	}

	val = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		printf("Failed to set to nonblock: %s\n", strerror(errno));
		goto err;
	}
	fcntl(fd, F_SETFD, FD_CLOEXEC);
	/* For loopback, we don't need UDP checksum */
	if (host == INADDR_LOOPBACK) {
		val = 1;
		setsockopt(fd, SOL_SOCKET, SO_NO_CHECK, &val, sizeof(val));
	}
	if (broadcast) {
		val = 1;
		rc = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
		if (rc < 0) {
			printf("%s", "setsockopt failed");
			goto err;
		}
	}
	if (reuse) {
		val = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&val, sizeof(val)) < 0) {
			printf("%s", "setsockopt SO_REUSEADDR");
			goto err;
		}

	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if(host == INADDR_LOOPBACK)
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_pton(AF_INET, host, (void *)&addr.sin_addr);
	rc = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		printf("Failed to bind: %s\n", strerror(errno));
		goto err;
	}

	return fd;

err:
	close(fd);
	return -1;
}

int udp_connect(unsigned int host, int port)
{
	int fd;
	int rc, val;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		printf("socket failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if(host == INADDR_LOOPBACK)
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

	rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		printf("connect fail: %s\n", strerror(errno));
		goto err;
	}

	val = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		printf("set nonblock fail: %s\n", strerror(errno));
		goto err;
	}
	fcntl(fd, F_SETFD, FD_CLOEXEC);

	val = 1;
	setsockopt(fd, SOL_SOCKET, SO_NO_CHECK, &val, sizeof(val));

	return fd;

err:
	close(fd);
	return -1;
}

int udp_create(void)
{
	int fd, val;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd < 0) {
		printf("socket failed: %s\n", strerror(errno));
		return -1;
	}

	val = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		printf("set nonblock fail: %s\n", strerror(errno));
		goto err;
	}
	fcntl(fd, F_SETFD, FD_CLOEXEC);

	val = 1;
	setsockopt(fd, SOL_SOCKET, SO_NO_CHECK, &val, sizeof(val));

	return fd;

err:
	close(fd);
	return -1;
}

int udp_sendto(int sock_fd,  unsigned long host, int port, unsigned char* snd_buf, int snd_len)
{
	int                ret;
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_addr.s_addr = host;
	server_addr.sin_port = htons(port);
	ret = sendto(sock_fd, snd_buf, snd_len, 0, (struct sockaddr *)&(server_addr), sizeof(server_addr));
	if (ret == -1) {
		printf("sendto err!\n");
	}
	return ret;
}
