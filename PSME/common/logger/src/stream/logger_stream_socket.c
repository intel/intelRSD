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
 *
 * @file logger_stream_socket.c
 *
 * @brief Logger stream socket implementation
 * */

#include "logger_stream_socket.h"

#include "logger_assert.h"
#include "logger_memory.h"

#include <safe-string/safe_lib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

/*!
 * Socket error message
 * */
LOGGER_PRINTF_FORMAT(1, 2)
static int socket_error(const char *format, ...) {

    int ret;
    va_list args;

    va_start(args, format);
    ret = vfprintf(stderr, format, args);
    va_end(args);

    return ret;
}

/*!
 * @struct logger_stream_setting_socket
 * @brief Settings for socket stream
 *
 * @var logger_stream_setting_socket::fd
 * File description for socket
 *
 * @var logger_stream_setting_socket::connected
 * Boolean flag indicates that socket is connected/open
 *
 * @var logger_stream_setting_socket::address
 * Socket address information
 * */
struct logger_stream_setting_socket {
    int fd;
    bool connected;
    struct sockaddr address;
};

static const struct timeval g_udp_receive_timeout = {
    .tv_sec = 0,
    .tv_usec = LOGGER_STREAM_UDP_RECEIVE_TIMEOUT_US
};

static int logger_stream_open_socket(struct logger_stream *inst,
        const char *ip_address, uint16_t port) {

    int err;
    struct logger_stream_setting_socket *sock =
        logger_memory_alloc(sizeof(struct logger_stream_setting_socket));

    if (NULL == sock) {
        return LOGGER_ERROR_MEMORY_OUT;
    }

    memset(sock, 0, sizeof(struct logger_stream_setting_socket));

    struct sockaddr_in server_address;

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = (htons)(port);
    memcpy_s(&sock->address, sizeof(struct sockaddr), &server_address, sizeof(struct sockaddr));

    err = logger_stream_add_message(inst, sock, LOGGER_MESSAGE_OPEN);
    if (LOGGER_SUCCESS != err) {
        logger_memory_free(sock);
        return err;
    }

    return LOGGER_SUCCESS;
}

int logger_stream_open_udp(struct logger_stream *inst,
        const char *ip_address, uint16_t port) {
    logger_assert(NULL != inst);
    logger_assert(NULL != ip_address);
    if (LOGGER_STREAM_UDP != inst->type) return LOGGER_ERROR_TYPE;
    return logger_stream_open_socket(inst, ip_address, port);
}

int logger_stream_open_tcp(struct logger_stream *inst,
        const char *ip_address, uint16_t port) {
    logger_assert(NULL != inst);
    logger_assert(NULL != ip_address);
    if (LOGGER_STREAM_TCP != inst->type) return LOGGER_ERROR_TYPE;
    return logger_stream_open_socket(inst, ip_address, port);
}

int logger_stream_socket_create(struct logger_stream *inst) {
    logger_assert(NULL != inst);
    if (NULL == inst->settings) return LOGGER_ERROR_NULL;

    logger_memory_free(inst->buffer);
    inst->buffer = logger_memory_alloc(LOGGER_DEFAULT_SOCKET_SIZE);
    inst->buffer_size = LOGGER_DEFAULT_SOCKET_SIZE;

    if (NULL == inst->buffer) {
        inst->buffer_size = 0;
        return LOGGER_ERROR_NULL;
    }

     return LOGGER_SUCCESS;
}

int logger_stream_socket_destroy(struct logger_stream *inst) {
    logger_assert(NULL != inst);

    struct logger_stream_setting_socket *sock = inst->settings;

    if (NULL != sock) {
        if (true == sock->connected) {
            close(sock->fd);
        }
    }

    logger_memory_free(sock);
    inst->settings = NULL;

    logger_memory_free(inst->buffer);
    inst->buffer = NULL;
    inst->buffer_size = 0;

    return LOGGER_SUCCESS;
}

int logger_stream_socket_udp_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);
    if (NULL == inst->settings) return LOGGER_ERROR_NULL;

    struct logger_stream_setting_socket *sock = inst->settings;
    ssize_t err;

    if (false == sock->connected) {
        sock->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock->fd < 0) {
            socket_error("Logger socket open: %s\n", strerror(errno));
            return LOGGER_ERROR;
        }

        err = setsockopt(sock->fd, SOL_SOCKET, SO_RCVTIMEO,
                &g_udp_receive_timeout, sizeof(struct timeval));
        if (err < 0) {
            socket_error("Logger socket udp option: %s\n", strerror(errno));
            close(sock->fd);
            return LOGGER_ERROR;
        }

        sock->connected = true;
    }

    err = sendto(sock->fd, inst->buffer, inst->index, 0,
            &sock->address, sizeof(struct sockaddr));
    if (err < 0) {
        socket_error("Logger socket send: %s\n", strerror(errno));
        sock->connected = false;
        close(sock->fd);
        return LOGGER_ERROR;
    }

    err = recvfrom(sock->fd, NULL, 0, 0, NULL, NULL);
    if (err < 0) {
        if (errno != EAGAIN) {
            socket_error("Logger socket receive: %s\n", strerror(errno));
            sock->connected = false;
            close(sock->fd);
            return LOGGER_ERROR;
        }
    }

    return LOGGER_SUCCESS;
}

int logger_stream_socket_tcp_flush(struct logger_stream *inst) {
    logger_assert(NULL != inst);
    if (NULL == inst->settings) return LOGGER_ERROR_NULL;

    struct logger_stream_setting_socket *sock = inst->settings;
    ssize_t err;

    if (false == sock->connected) {
        sock->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock->fd < 0) {
            socket_error("Logger socket open: %s\n", strerror(errno));
            return LOGGER_ERROR;
        }

        err = connect(sock->fd, &sock->address, sizeof(struct sockaddr));
        if (err < 0) {
            if (ECONNREFUSED != errno) {
                socket_error("Logger socket connect: %s\n", strerror(errno));
            }
            close(sock->fd);
            return LOGGER_ERROR;
        }

        sock->connected = true;
    }

    err = write(sock->fd, inst->buffer, inst->index);
    if (err < 0) {
        if (ECONNRESET != errno) {
            socket_error("Logger socket write: %s\n", strerror(errno));
        }
        sock->connected = false;
        close(sock->fd);
        return LOGGER_ERROR;
    }

    return LOGGER_SUCCESS;
}
