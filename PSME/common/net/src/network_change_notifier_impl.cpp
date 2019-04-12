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
 * @file network_change_notifier.cpp
 *
 * @brief NetworkChangeNotifier implementation.
 * */

#include "net/network_change_notifier_impl.hpp"

#include "net/ipaddress.hpp"
#include "logger/logger_factory.hpp"
#include <safe-string/safe_lib.hpp>

namespace net {

struct NetlinkRequest {
    struct nlmsghdr header;
    struct rtgenmsg msg;
};

namespace {

/*!
 * @brief Exception thrown when error condition occurs
 * while processing netlink message.
 */
class NetworkChangeNotifierException : public std::runtime_error {
public:
    /*!
     * @brief Constructor
     * @param msg Error description
     */
    explicit NetworkChangeNotifierException(const std::string& msg)
        : std::runtime_error("NetworkChangeNotifier - " + msg) {}

    /*!
     * @brief Constructor
     */
    NetworkChangeNotifierException(const NetworkChangeNotifierException&) = default;

    /*! @brief Destructor */
    virtual ~NetworkChangeNotifierException();
};

NetworkChangeNotifierException::~NetworkChangeNotifierException() {}

// Ignores spurious messages, where ifi_change == 0 and rta_type == IFLA_WIRELESS
bool ignore_wireless_change(const struct nlmsghdr* header, const struct ifinfomsg* msg) {
    size_t length = IFLA_PAYLOAD(header);
    for (const struct rtattr* attr = IFLA_RTA(const_cast<struct ifinfomsg*>(msg));
            RTA_OK(attr, length);
            attr = RTA_NEXT(const_cast<struct rtattr*>(attr), length)) {
        if (attr->rta_type == IFLA_WIRELESS && msg->ifi_change == 0) {
            return true;
        }
    }
    return false;
}

// Retrieves IP address from NETLINK message.
// Sets is_deprecated for IPv6 addresses with preferred lifetimes of 0.
bool get_ipaddress(const struct nlmsghdr* header, IpAddress& ip_address, bool* is_deprecated) {
    if (is_deprecated) {
        *is_deprecated = false;
    }
    const struct ifaddrmsg* msg =
            reinterpret_cast<struct ifaddrmsg*>(NLMSG_DATA(const_cast<nlmsghdr*>(header)));
    void* address = NULL;
    void* local = NULL;
    size_t length = IFA_PAYLOAD(header);
    for (const struct rtattr* attr =
            reinterpret_cast<const struct rtattr*>(IFA_RTA(const_cast<struct ifaddrmsg*>(msg)));
            RTA_OK(attr, length);
            attr = RTA_NEXT(const_cast<struct rtattr*>(attr), length)) {
        switch (attr->rta_type) {
        case IFA_ADDRESS:
            address = RTA_DATA(const_cast<struct rtattr*>(attr));
            break;
        case IFA_LOCAL:
            local = RTA_DATA(const_cast<struct rtattr*>(attr));
            break;
        case IFA_CACHEINFO:
        {
            const struct ifa_cacheinfo *cache_info =
                    reinterpret_cast<const struct ifa_cacheinfo*>(RTA_DATA(const_cast<struct rtattr*>(attr)));
            if (is_deprecated) {
                *is_deprecated = (cache_info->ifa_prefered == 0);
            }
        }
            break;
        default:
            break;
        }
    }
    if (local) {
        address = local;
    }
    if (!address) {
        return false;
    }
    if (AF_INET == msg->ifa_family) {
        ip_address = IpAddress(*reinterpret_cast<const in_addr*>(address));
    }
    else if (AF_INET6 == msg->ifa_family) {
        ip_address = IpAddress(*reinterpret_cast<const in6_addr*>(address));
    }
    return true;
}

void init_shutdown_pipe(int fds[2]) {
    if (pipe(fds) < 0) {
        throw NetworkChangeNotifierException("Could not create pipe");
    }
    if (fcntl(fds[0], F_SETFL, O_NONBLOCK) < 0) {
        throw NetworkChangeNotifierException("Could not set non-blocking mode on pipe read end");
    }
    if (fcntl(fds[1], F_SETFL, O_NONBLOCK) < 0) {
        throw NetworkChangeNotifierException("Could not set non-blocking mode on pipe write end");
    }
}

int create_netlink_socket() {
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        throw NetworkChangeNotifierException("Could not create NETLINK socket");
    }
    return fd;
}

} // namespace


NetworkChangeNotifierImpl::NetworkChangeNotifierImpl() {
    init();
}

NetworkChangeNotifierImpl::~NetworkChangeNotifierImpl() {
    close_descriptors();
}

void NetworkChangeNotifierImpl::start() {
    if (!m_is_running) {
        m_is_running = true;
        m_thread = std::thread(&NetworkChangeNotifierImpl::execute, this);
    }
}

void NetworkChangeNotifierImpl::stop() {
    if (m_is_running) {
        send_shutdown_msg();
        m_is_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void NetworkChangeNotifierImpl::send_shutdown_msg() {
    if (m_shutdown_fd[1] != -1) {
        const uint8_t b[1] = {0};
        if (write(m_shutdown_fd[1], b, sizeof (b)) < 0) {
            log_error("net", "Failed to notify NetworkChangeNotifier to close.");
        }
    }
}

void NetworkChangeNotifierImpl::execute() {
    log_info("net", "NetworkChangeNotifier started.");
    while (m_is_running) {
        fd_set read_set;
        FD_ZERO(&read_set);
        int max_fd = 0;
        if (m_netlink_fd != -1) {
            FD_SET(m_netlink_fd, &read_set);
            if (m_netlink_fd > max_fd) {
                max_fd = m_netlink_fd;
            }
        }
        else {
            log_warning("net", "NetworkChangeNotifier - no NETLINK socket to monitor.");
            m_is_running = false;
            close_descriptors();
            break;
        }
        if (m_shutdown_fd[0] != -1) {
            FD_SET(m_shutdown_fd[0], &read_set);
            if (m_shutdown_fd[0] > max_fd) {
                max_fd = m_shutdown_fd[0];
            }
        }
        auto rc = ::select(max_fd + 1, &read_set, nullptr, nullptr, nullptr);
        if (rc > 0) {
            if (m_netlink_fd != -1 && FD_ISSET(m_netlink_fd, &read_set)) {
                try {
                    process_netlink_messages_and_notify();
                }
                catch(...) {
                    log_error("net", "NetworkChangeNotifier - Unknown error");
                }
            }
            if (m_shutdown_fd[0] != -1 && FD_ISSET(m_shutdown_fd[0], &read_set)) {
                uint8_t b[1];
                auto ret = read(m_shutdown_fd[0], b, sizeof(b));
                if (ret < 0) {
                    log_error("net", "NetworkChangeNotifier - Failed to read notification.");
                }
                m_is_running = false;
                break;
            }
        }
        else if (rc < 0 && EINTR != errno) {
            log_debug("net",
                      "NetworkChangeNotifier - select error: " << strerror(errno));
        }
    }
    log_debug("net", "NetworkChangeNotifier stopped.");
}

void NetworkChangeNotifierImpl::init() {

    init_shutdown_pipe(m_shutdown_fd);

    m_netlink_fd = create_netlink_socket();

    // Register for notifications.
    struct sockaddr_nl addr;
    memset_s(&addr, sizeof(sockaddr_nl), 0);
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_NOTIFY | RTMGRP_LINK;

    if (bind(m_netlink_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::string error = "Could not bind NETLINK socket: ";
        error += strerror(errno);
        close_descriptors();
        throw NetworkChangeNotifierException(error);
    }

    NetlinkRequest request;
    memset_s(&request, sizeof(NetlinkRequest), 0);
    request.header.nlmsg_len = NLMSG_LENGTH(sizeof(request.msg));
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_pid = getpid();
    request.msg.rtgen_family = AF_UNSPEC;

    // initialize address map
    request.header.nlmsg_type = RTM_GETADDR;
    send_netlink_request_and_process_response(request);

    // initialize online links
    request.header.nlmsg_type = RTM_GETLINK;
    send_netlink_request_and_process_response(request);
}

void NetworkChangeNotifierImpl::send_netlink_request_and_process_response(const NetlinkRequest& request) {
    struct sockaddr_nl peer;
    memset_s(&peer, sizeof(sockaddr_nl), 0);
    peer.nl_family = AF_NETLINK;
    int rc = 0;
    do {
        rc = int(sendto(m_netlink_fd, &request, request.header.nlmsg_len, 0,
                reinterpret_cast<struct sockaddr*>(&peer), sizeof(peer)));
    } while (rc == -1 && EINTR == errno);
    if (rc < 0) {
        std::string error = "Could not send NETLINK request: ";
        error += strerror(errno);
        close_descriptors();
        throw NetworkChangeNotifierException(error);
    }
    process_netlink_messages();
}

NetworkChangeNotifierImpl::ChangeList NetworkChangeNotifierImpl::process_netlink_messages() {
    char buffer[4096];
    bool first_iteration{true};
    ChangeList changes{};
    for (;;) {
        int rc = 0;
        do {
            rc = int(recv(m_netlink_fd, buffer, sizeof(buffer), first_iteration ? 0 : MSG_DONTWAIT));
        } while (rc == -1 && EINTR == errno);
        first_iteration = false;
        if (0 == rc) {
            log_error("net", "NETLINK socket has been shutdown.");
            break;
        }
        if (rc < 0) {
            if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                log_error("net", "Receive from NETLINK socket failed.");
            }
            break;
        }
        process_netlink_message(buffer, rc, changes);
    }
    return changes;
}

void NetworkChangeNotifierImpl::process_netlink_message(char* buffer, size_t length, ChangeList& changes) {

    for (struct nlmsghdr* header = reinterpret_cast<struct nlmsghdr*>(buffer);
            NLMSG_OK(header, length);
            header = NLMSG_NEXT(header, length)) {
        switch (header->nlmsg_type) {
        case NLMSG_DONE:
            return;
        case NLMSG_ERROR:
        {
            const struct nlmsgerr* msg =
                    reinterpret_cast<struct nlmsgerr*>(NLMSG_DATA(header));
            log_error("net", "Unexpected netlink error " << msg->error << ".");
        }
            return;
        case RTM_NEWADDR:
        {
            IpAddress address;
            bool is_deprecated;
            struct ifaddrmsg* msg =
                    reinterpret_cast<struct ifaddrmsg*>(NLMSG_DATA(header));
            if (get_ipaddress(header, address, &is_deprecated)) {
                // set the deprecated flag based on the preferred lifetime.
                if (is_deprecated) {
                    msg->ifa_flags |= IFA_F_DEPRECATED;
                }
                // indicate change if the address is new or ifaddrmsg has changed.
                AddressMap::iterator it = m_address_map.find(address);
                if (it == m_address_map.end()) {
                    m_address_map.emplace_hint(it, address, *msg);
                    changes.emplace_back(msg->ifa_index, NetworkChangeNotifier::ChangeType::NEW_IP);
                }
                else {
                    int diff = 0;
                    if ((EOK == memcmp_s(&it->second, sizeof(it->second), msg, sizeof(*msg), &diff))
                        && diff) {
                        it->second = *msg;
                        changes.emplace_back(msg->ifa_index, NetworkChangeNotifier::ChangeType::NEW_IP);
                    }
                }
            }
        }
            break;
        case RTM_DELADDR:
        {
            IpAddress address;
            const struct ifaddrmsg* msg =
                    reinterpret_cast<struct ifaddrmsg*>(NLMSG_DATA(header));
            if (get_ipaddress(header, address, nullptr)) {
                if (m_address_map.erase(address)) {
                    changes.emplace_back(msg->ifa_index, NetworkChangeNotifier::ChangeType::DEL_IP);
                }
            }
        }
            break;
        case RTM_NEWLINK:
        {
            const struct ifinfomsg* msg =
                    reinterpret_cast<struct ifinfomsg*>(NLMSG_DATA(header));
            if (ignore_wireless_change(header, msg)) {
                log_debug("net", "Ignoring RTM_NEWLINK message");
                break;
            }
            if (!(msg->ifi_flags & IFF_LOOPBACK) && (msg->ifi_flags & IFF_UP)
                 && (msg->ifi_flags & IFF_RUNNING)) {
                if (m_online_links.emplace(unsigned(msg->ifi_index)).second) {
                    changes.emplace_back(msg->ifi_index, NetworkChangeNotifier::ChangeType::NEW_LINK);
                }
            }
            else {
                if (m_online_links.erase(msg->ifi_index)) {
                    changes.emplace_back(msg->ifi_index, NetworkChangeNotifier::ChangeType::DEL_LINK);
                }
            }
        }
            break;
        case RTM_DELLINK:
        {
            const struct ifinfomsg* msg = reinterpret_cast<struct ifinfomsg*>(NLMSG_DATA(header));
            if (m_online_links.erase(msg->ifi_index)) {
                changes.emplace_back(msg->ifi_index, NetworkChangeNotifier::ChangeType::DEL_LINK);
            }
        }
            break;
        default:
            break;
        }
    }
}

void NetworkChangeNotifierImpl::process_netlink_messages_and_notify() {
    const auto change_stack = process_netlink_messages();
    for (const auto& change: change_stack) {
        notify_listeners(std::get<0>(change), std::get<1>(change));
    }
}

void NetworkChangeNotifierImpl::close_descriptors() {
    if (m_netlink_fd >= 0 && close(m_netlink_fd) < 0) {
        log_error("net", "Could not close NETLINK socket.");
    }
    m_netlink_fd = -1;
    if (m_shutdown_fd[0] >= 0) {
        close(m_shutdown_fd[0]);
        m_shutdown_fd[0] = -1;
    }
    if (m_shutdown_fd[1] >= 0) {
        close(m_shutdown_fd[1]);
        m_shutdown_fd[1] = -1;
    }
}

}
