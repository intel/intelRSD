/*!
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
 *
 * @file socket_defs.hpp
 *
 * @brief common includes and constants for net
 * */

#pragma once

#define NET_HAVE_IPv6 1
#define NET_OS_FAMILY_UNIX 1
#define NET_ENOERR 0

#include "net_exception.hpp"

extern "C" {
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <net/if.h>
}

using net_socket_t = int;
constexpr const net_socket_t NET_INVALID_SOCKET = -1;
using net_socklen_t = socklen_t;
using net_fcntl_cmd_t = int;
using net_ioctl_request_t = int;

namespace net {

    /*!
     * Possible address families for socket addresses.
     */
    enum class AddressFamily {
        /*! IPv4 address family. */
        IPv4,
#ifdef NET_HAVE_IPv6
        /*! IPv6 address family. */
        IPv6,
#endif
        /*! UNIX domain socket address family. */
        UNIX_LOCAL
    };

} // namespace net
