/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

#include "mock_clif.hpp"

#include <stdexcept>

static bool no_conn = false;
static std::string m_reply{};

void mock_expect_no_conn() {
    no_conn = true;
}

void mock_expect_reply(const std::string& r) {
    m_reply = r;
}

int socket(int, int, int) noexcept {
    if (no_conn) {
        no_conn = false;
        return -1;
    }
    // create a temp file for reading and writing
    // the file will not appear in the filesystem
    int fd = open("/tmp", O_TMPFILE | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (0 > write(fd, m_reply.c_str(), m_reply.size())) {
        throw std::runtime_error("Failed writing to temp file");
    }
    lseek(fd, SEEK_SET, 0);
    return fd;
}

int connect(int, const struct sockaddr *, socklen_t) {
    return 0;
}
