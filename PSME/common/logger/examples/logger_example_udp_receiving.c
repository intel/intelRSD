/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 * */
#include <safe-string/safe_lib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define UDP_ADDRESS         "127.0.0.1"
#define UDP_PORT            8888

#define BUFFER_SIZE         4096

static char buffer[BUFFER_SIZE];

int main(void) {
    struct sockaddr_in socket_address;
    struct sockaddr sock_address;

    socklen_t sock_address_size = sizeof(sock_address);
    ssize_t recv_len = 0;
    ssize_t send_len = 0;
    int err = 0;

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        return sockfd;
    }

    bzero(&socket_address, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(UDP_ADDRESS);
    socket_address.sin_port = htons(UDP_PORT);
    memcpy_s(&sock_address, sizeof(sock_address), &socket_address, sizeof(sock_address));

    err = bind(sockfd, &sock_address, sizeof(sock_address));
    if (err < 0) {
        return err;
    }

    for (;;) {
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                &sock_address, &sock_address_size);
        if (recv_len < 0) {
            fprintf(stderr, "Socket recvfrom: %s\n", strerror(errno));
            close(sockfd);
            return (int)recv_len;
        }

        send_len = sendto(sockfd, NULL, 0, 0, &sock_address, sock_address_size);
        if (send_len < 0) {
            fprintf(stderr, "Socket sendto: %s\n", strerror(errno));
            close(sockfd);
            return (int)send_len;
        }

        for (int i = 0; i < recv_len; i++) {
            putchar(buffer[i]);
        }
    }
}
