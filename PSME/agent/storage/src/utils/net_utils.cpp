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
 * */

#include "utils/net_utils.hpp"
#include "logger/logger_factory.hpp"

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>

using std::string;
using namespace agent::storage::utils;

void NetUtils::IfAddrsDeleter::operator()(struct ifaddrs *ifa) const {
    freeifaddrs(ifa);
}

const string NetUtils::get_ip_address(const string& interface_name) {
    const int wait_seconds = 10;
    while (true) {
        struct ifaddrs *ifa = nullptr;
        IfAddrsPointer ifaddrs{};
        if (0 == getifaddrs(&ifa)) {
            for (ifaddrs.reset(ifa); ifa; ifa = ifa->ifa_next) {
                if (AF_INET == ifa->ifa_addr->sa_family
                    && 0 == interface_name.compare(ifa->ifa_name)) {
                    auto sock_addr = reinterpret_cast<
                                        struct sockaddr_in *>(ifa->ifa_addr);
                    auto ipaddress = inet_ntoa(sock_addr->sin_addr);
                    if (nullptr != ipaddress) {
                        return ipaddress;
                    }
                }
            }
        }
        log_error(GET_LOGGER("agent-storage"),
                    "Could not read IP address from interface " << interface_name);
        std::this_thread::sleep_for(std::chrono::seconds(wait_seconds));
    }
}
