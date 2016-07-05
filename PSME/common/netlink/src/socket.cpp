/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file socket.cpp
 *
 * @brief Netlink socket implementation.
 * */

#include "netlink/socket.hpp"
#include "netlink/exceptions.hpp"

#include <linux/netdevice.h>

using namespace netlink_base;

static const int BUFLEN = 4096;
struct rcvbuf_t {
    struct nlmsghdr hdr;
    struct ifinfomsg ifi;
    char data[BUFLEN];
};

Socket::Socket() : m_sock(nl_socket_alloc()) {}

Socket::~Socket() {}

void Socket::connect() {
    if (!m_sock) {
        throw std::runtime_error("null socket");
    }

    /* binds and connects to the socket of NETLINK_ROUTE protocol */
    if (nl_connect(m_sock.get(), NETLINK_ROUTE) != 0) {
        throw std::runtime_error("nl_connect failed");
    }
}

void Socket::send_message(const Message& message) {
    auto nlmsg = message.prepare_netlink_msg();
    if (nullptr == nlmsg) {
        throw std::runtime_error("null netlink message");
    }
    if (0 > nl_send_auto(m_sock.get(), nlmsg.get())) {
        throw std::runtime_error("nl_send_auto failed");
    }
}

void Socket::send_rtnl_message(Message& message) {
    auto nlmsg = message.prepare_rtnl_msg();
    if (nullptr == nlmsg) {
        throw std::runtime_error("null netlink message");
    }
    if (0 > nl_send_auto(m_sock.get(), nlmsg.get())) {
        throw std::runtime_error("nl_send_auto failed");
    }
}

void Socket::receive_link_state(Message &message) {
    struct rcvbuf_t rcvbuf{};
    int sockfd;
    ssize_t nLen;

    sockfd = nl_socket_get_fd(m_sock.get());
    while ((nLen = recv(sockfd, &rcvbuf, sizeof(rcvbuf), 0)) > 0) {
        const auto& nlh = &rcvbuf.hdr;
        if (NLMSG_OK(nlh, nLen)) {
            if (NLMSG_ERROR == nlh->nlmsg_type) {
                throw std::runtime_error("error on receive");
            }
            if (rcvbuf.ifi.ifi_flags & IFF_UP) {
                message.set_link_state(State::Up);
            }
            else {
                message.set_link_state(State::Down);
            }
            if (rcvbuf.ifi.ifi_flags & IFF_RUNNING) {
                message.set_operational_state(State::Up);
            }
            else {
                message.set_operational_state(State::Down);
            }
            break;
        }
    }
    if (nLen <= 0) {
        throw std::runtime_error("recv failed");
    }
}

void Socket::receive_message(Message &message) {
    std::unique_ptr<uint8_t[]> data(new uint8_t[RECV_MESSAGE_SIZE]);
    struct sockaddr_nl nladdr{};
    struct iovec iov{};
    struct msghdr msg{};
    int sockfd{-1};
    int msglen{0};
    struct nlmsghdr* nl_header = nullptr;

    msg.msg_name = &nladdr;
    msg.msg_namelen = sizeof(nladdr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1; /* only one message */
    iov.iov_base = data.get();

    /* receive message loop */
    sockfd = nl_socket_get_fd(m_sock.get());
    while (true) {
        iov.iov_len = RECV_MESSAGE_SIZE;
        auto status = recvmsg(sockfd, &msg, 0);
        if (0 > status) {
            if (EINTR == errno || EAGAIN == errno) {
                continue;
            }
            throw std::runtime_error("recvmsg() failed");
        }

        if (0 == status) {
            throw std::runtime_error("recvmsg() EOF on netlink");
        }

        nl_header = reinterpret_cast<struct nlmsghdr*>(data.get());
        msglen = int(status);

        while (nlmsg_ok(nl_header, msglen)) {
            if (0 == nladdr.nl_pid) {
                if (nl_header->nlmsg_flags & NLM_F_DUMP_INTR) {
                    log_debug(GET_LOGGER("netlink"),
                              "Dump was interrupted and may be inconsistent");
                }
                if (nl_header->nlmsg_type == NLMSG_DONE) {
                    return;
                }
                if (nl_header->nlmsg_type == NLMSG_ERROR) {
                    struct nlmsgerr* err = static_cast<struct nlmsgerr*>
                                                       (nlmsg_data(nl_header));
                    if (int(nl_header->nlmsg_len) <
                        nlmsg_size(sizeof(struct nlmsgerr))) {
                        throw std::runtime_error("Netlink message truncated");
                    }
                    if (0 == err->error) {
                        /* ACK messages received */
                        return;
                    }
                    else {
                        /* throw netlink error message exception */
                        throw_error(err->error);
                    }
                }
                else {
                    /* parse netlink header */
                    Message::NetlinkHeader nlhdr(nl_header);
                    message.parse_netlink_hdr(nlhdr);
                }
            }
            nl_header = nlmsg_next(nl_header, &msglen);
        }

        if (msg.msg_flags & MSG_TRUNC) {
            log_debug(GET_LOGGER("netlink"), "Message truncated");
        }
        if (msglen) {
            log_debug(GET_LOGGER("netlink"), "Remnant of size");
        }
    }
}

bool Socket::modify_callback(nl_recvmsg_msg_cb_t func, void *arg) {
    int ret;

    ret = nl_socket_modify_cb(m_sock.get(), NL_CB_VALID, NL_CB_CUSTOM, func,
                              arg);
    if (0 != ret)
        return false;
    ret = nl_socket_modify_cb(m_sock.get(), NL_CB_FINISH, NL_CB_CUSTOM, func,
                              arg);
    if (0 != ret)
        return false;
    return true;
}

void Socket::receive_to_callback(Message &message) {
    while(!message.get_last()) {
        auto err = nl_recvmsgs_default(m_sock.get());
        if (0 > err) {
            break;
        }
    }
}

void Socket::throw_error(const int error_code) const {
    switch (error_code) {
        case -EINVAL:
            throw NetlinkInvalidParamsException();
        default:
            throw NetlinkException(error_code);
    }
}

