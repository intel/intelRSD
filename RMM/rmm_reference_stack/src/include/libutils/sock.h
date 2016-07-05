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


#ifndef __LIBUTILS_SOCK_H__
#define __LIBUTILS_SOCK_H__

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

int create_udp_listen(unsigned int host, int port, int broadcast, int reuse);
int udp_create(void);
int udp_connect(unsigned int host, int port);
int udp_sendto(int sock_fd,  unsigned long host, int port, unsigned char* snd_buf, int snd_len);
int udp_recv(int sock_fd, unsigned char* recv_buf, int len, int timeout_ms);
int ping(int host, int timeout);


static inline ssize_t socket_recv(int sockfd, void *buf, size_t len)
{
	ssize_t tmp;
	ssize_t total = 0;
	int retry = 0;

	while (retry < 3) {
		tmp = recv(sockfd, buf+total, len-total, 0);
		if (tmp < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				usleep(1000);
				retry++;
				continue;
			}
			return -1;
		} else if (tmp > 0) {
			total += tmp;
		} else
			break;
	}

	return total;
}

static inline ssize_t socket_send(int sockfd, const void *buf, size_t len)
{
	ssize_t tmp;
	size_t left = len;
	const char *p = buf;

	while(1) {
		tmp = send(sockfd, p, left, 0);
		if(tmp < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				usleep(1000);
				continue;
			}
			return -1;
		}

		if((size_t)tmp == left)
			return len;
		left -= tmp;
		p += tmp;
	}

	return tmp;

}


#endif
