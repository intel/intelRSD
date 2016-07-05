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

static unsigned short inet_cksum(unsigned short *addr, int len)
{
	unsigned int sum = 0;   

	while (len > 1) {
		sum += *addr++;
		len -= 2;
	}

	if (len == 1) {
		unsigned short pad;

		*(unsigned char *)&pad = *(unsigned char *)addr;
		sum += pad;
	}

	sum  = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	return (unsigned short) ~sum;
}

static void ping_host_by_icmp(int icmp_fd, unsigned int host)
{
	static int init = 0;
	static struct sockaddr_in dest;
	static unsigned char packet[ICMP_MINLEN + 4];

	if (host == 0 || host == 0xFFFFFFFF)
		return;

	if (init == 0) {
		struct icmp *icmphdr;

		init = 1;

		dest.sin_family = AF_INET;

		icmphdr = (struct icmp *)packet;
		icmphdr->icmp_type = ICMP_ECHO;
		icmphdr->icmp_cksum = inet_cksum((unsigned short *)packet, sizeof(packet));
	}

	dest.sin_addr.s_addr = host;

	sendto(icmp_fd, packet, sizeof(packet), 0, (struct sockaddr *)&dest, sizeof(dest));
}

static int open_icmp_socket(int timeout)
{
	int fd;
	fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (fd < 0) {
		perror("socket() failed");
		return -1;
	}
	struct timeval tv;

	tv.tv_sec = timeout;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

	return fd;

}

static int process_ping_from_icmp(int icmp_fd, unsigned int host)
{
	int rc;
	unsigned char packet[512];
	memset(packet, 0, 512);
	rc = socket_recv(icmp_fd, packet, sizeof(packet));

	if (rc < sizeof(struct iphdr)) {
		printf("socket recv fail\n");
		return -1;  
	}

	struct iphdr *ip = (struct iphdr *)packet;
	if(ip->saddr == 0 || ip->saddr != host) {
		printf("ip: %d fail\n", ip->saddr);
		return -1;
	}

	return 0;
}

int ping(int host, int timeout)
{
	int ret = -1;
	//int host = inet_addr(ip);

	int fd = open_icmp_socket(timeout);
	if(fd < 0)
		return -1;

	ping_host_by_icmp(fd, host);
	ret = process_ping_from_icmp(fd, host);

	close(fd);
	return ret;
}
