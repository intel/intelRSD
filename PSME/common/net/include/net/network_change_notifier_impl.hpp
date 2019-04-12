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
 * @file network_change_notifier_impl.hpp
 *
 * @brief NetworkChangeNotifier implementation
 * */

#pragma once

#include "net/network_change_notifier.hpp"
#include "net/ipaddress.hpp"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

extern "C" {
#include <linux/rtnetlink.h>
}

namespace net {

struct NetlinkRequest;

/*!
 * NetworkChangeNotifier implementation using rtnetlink.
 */
class NetworkChangeNotifierImpl : public NetworkChangeNotifier {
public:
    /* Constructor */
    NetworkChangeNotifierImpl();

    /*! Destructor */
    ~NetworkChangeNotifierImpl() override;

    void start() override;

    void stop() override;

private:
    friend class NetworkChangeNotifierTest;
    std::atomic<bool> m_is_running{false};
    std::thread m_thread{};

    void init();
    void send_netlink_request_and_process_response(const NetlinkRequest& request);
    void execute();

    using AddressMap = std::map<IpAddress, struct ifaddrmsg>;
    AddressMap m_address_map{};
    std::unordered_set<unsigned> m_online_links{};

    int m_netlink_fd{-1};
    void close_descriptors();
    int m_shutdown_fd[2] = {-1,-1}; // self-pipe trick
    void send_shutdown_msg();

    using Change = std::tuple<unsigned, NetworkChangeNotifier::ChangeType>;
    using ChangeList = std::vector<Change>;
    void process_netlink_messages_and_notify();
    ChangeList process_netlink_messages();
    void process_netlink_message(char* buffer, size_t length, ChangeList& changes);
};

}
