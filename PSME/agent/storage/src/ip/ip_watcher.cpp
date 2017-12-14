
/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/module/storage_components.hpp"
#include "ip/ip_watcher.hpp"



extern "C" {
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
}

using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent_framework::model;
using namespace agent::storage;
using namespace agent::storage::ip_discovery;

namespace {
constexpr int TIMEOUT_S = 5;
const std::string OLD{"OLD"};
}


void IpWatcher::handle_ip_change(const std::string& new_ip) {
    try {
        IpManager ip_manager{};
        ip_manager.update_portal_ip(new_ip);
    }
    catch (const std::exception& error) {
        log_debug(GET_LOGGER("storage-agent"), "Updating Portal IP exception occured: "
            << error.what());
    }
}


const std::string IpWatcher::detect_ip() {
    auto& target_manager = get_manager<IscsiTarget, IscsiTargetManager>();
    auto& iscsi_data = target_manager.get_iscsi_data();
    struct ifaddrs* ifa = nullptr;
    struct ifaddrs* temp_ifa = nullptr;
    std::string ip{};
    if (0 == getifaddrs(&ifa)) {
        for (temp_ifa = ifa; temp_ifa; temp_ifa = temp_ifa->ifa_next) {
            if (AF_INET == temp_ifa->ifa_addr->sa_family
                && 0 == iscsi_data.get_portal_interface().compare(temp_ifa->ifa_name)) {
                auto sock_addr = reinterpret_cast<struct sockaddr_in*>(temp_ifa->ifa_addr);
                auto ipaddress = inet_ntoa(sock_addr->sin_addr);
                if (nullptr == ipaddress) {
                    ip = "";
                    break;
                }
                if (0 != iscsi_data.get_portal_ip().compare(ipaddress)) {
                    ip = ipaddress;
                }
                else {
                    ip = OLD;
                };
                break;
            }
        }
        freeifaddrs(ifa);
    }
    return ip;
}


void IpWatcher::execute() {
    while (is_running()) {
        sleep(TIMEOUT_S);
        auto ip = detect_ip();
        if (0 != ip.compare(OLD)) {
            handle_ip_change(ip);
        }
    }
}


IpWatcher::~IpWatcher() {}
