/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

#include "mock_sockets.hpp"

#include <stdexcept>

static bool fail_on_socket{false};
static bool fail_on_connect{false};
static bool fail_on_write{false};
static bool fail_on_read{false};
static int command_status{};
static bool get_node_id{false};
static bool get_node_details{false};

static void my_write(int fd, void* p, int count) {
    if (count != write(fd, p, count)) {
        throw std::runtime_error("Failed writing to temp file");
    }
}

static void fill_neighbors(int fd) {
    char buf[] = "Neighbor\0\0\0\0\0\0";
    char command[] = "GetNodeNeighbors ";
    int len = 0;
    int status = htonl(command_status);
    my_write(fd, &len, sizeof(int));
    my_write(fd, command, sizeof(command));
    len = htonl(sizeof(buf) + 2 * sizeof(int));
    my_write(fd, &len, sizeof(int));
    my_write(fd, &status, sizeof(int));
    len = 1;
    my_write(fd, &len, sizeof(int));
    my_write(fd, buf, sizeof(buf));
}

static void fill_node_id(int fd) {
    char buf[] = "Node1\0\0\0\0\0\0";
    char command[] = "GetNodeId";
    int len = 0;
    my_write(fd, &len, sizeof(int));
    my_write(fd, command, sizeof(command));
    len = htonl(sizeof(buf) + 2 * sizeof(int));
    my_write(fd, &len, sizeof(int));
    int status = htonl(command_status);
    my_write(fd, &status, sizeof(int));
    len = 1;
    my_write(fd, &len, sizeof(int));
    my_write(fd, buf, sizeof(buf));
}

struct switch_info {
    unsigned char mac[ETH_ALEN];
    struct in_addr ipv4addr;
    int32_t metric;
    uint32_t num_nexthop;
} __attribute__((packed));

static void fill_node_details(int fd) {
    char buf[] = "Neighbor\0\0\0\0\0\0";
    char command[] = "GetNodeDetails Neighbor";
    int len = 0;
    int status = htonl(command_status);
    my_write(fd, &len, sizeof(int));
    my_write(fd, command, sizeof(command));
    len = htonl(sizeof(buf) + 2 * sizeof(int));
    my_write(fd, &len, sizeof(int));
    my_write(fd, &status, sizeof(int));
    switch_info si{};
    si.num_nexthop = 3;
    my_write(fd, &si, sizeof(si));
    for (unsigned int i = 0; i < si.num_nexthop; i++) {
        int type{};
        char hop[] = "Hop X";
        std::snprintf(hop, sizeof(hop), "Hop %d", i + 1 );
        my_write(fd, &type, sizeof(type));
        my_write(fd, &hop, sizeof(hop));
    }
}

int socket(int, int, int) noexcept {
    if (fail_on_socket) {
        return -1;
    }
    if (fail_on_write) {
        // open some read-only file
        return open("/dev/random", O_RDONLY);
    }
    if (fail_on_read) {
        // open some write-only file
        return open("/dev/null", O_WRONLY);
    }
    // create a temp file for reading and writing
    // the file will not appear in the filesystem
    int fd = open("/tmp", O_TMPFILE | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (get_node_id) {
        fill_node_id(fd);
    }
    else if (get_node_details) {
        fill_node_details(fd);
    }
    else {
        fill_neighbors(fd);
    }
    lseek(fd, SEEK_SET, 0);
    return fd;
}

int connect(int, const struct sockaddr *, socklen_t) {
    return fail_on_connect ? -1 : 0;
}

void expect_fail_on_socket(bool fail) {
    fail_on_socket = fail;
}

void expect_fail_on_connect(bool fail) {
    fail_on_connect = fail;
}

void expect_fail_on_write(bool fail) {
    fail_on_write = fail;
}

void expect_fail_on_read(bool fail) {
    fail_on_read = fail;
}

void expect_command_status(int status) {
    command_status = status;
}

void expect_get_node_id(bool value) {
    get_node_id = value;
}

void expect_get_node_details(bool value) {
    get_node_details = value;
}
