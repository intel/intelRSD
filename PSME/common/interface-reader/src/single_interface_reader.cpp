/*!
 * @brief Interface reader implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file single_interface_reader.cpp
 */

#include "interface-reader/single_interface_reader.hpp"
#include "net/network_interface.hpp"
#include "logger/logger.hpp"


using namespace interface_reader;

SingleInterfaceReader::~SingleInterfaceReader() {}

std::vector<std::string> SingleInterfaceReader::get_interfaces() const {
    auto interfaces = std::vector<std::string>{};

    const auto ifaces = net::NetworkInterface::get_interfaces();
    log_debug("interface-reader", "Trying to match network interface: '" << m_interface_name << "'");
    for (const auto& ifa : ifaces) {
        log_debug("interface-reader", "Reading interface: " << ifa.get_name());
        if (ifa.get_name() == m_interface_name) {
            interfaces.emplace_back(ifa.get_name());
        }
    }
    return interfaces;
}

