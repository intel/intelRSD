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
 * @file port.hpp
 *
 * @brief Switch port utils
 * */

#pragma once

#include <string>
#include "json-wrapper/json-wrapper.hpp"

namespace agent {
namespace network {
namespace utils {

/*!
 * @brief Init vlans on switch port
 *
 * @param[in] port_identifier Port identifier
 */
void init_switch_vlan_port(const std::string& port_identifier);

/*!
 * @brief Get port UUID by given port identifier
 *
 * @param[in] port_identifier Port identifier
 * @param[out] port_uuid Port UUID (if port exists)
 *
 * @return true if port exists otherwise false
 */
bool get_port_uuid_by_identifier(const std::string& port_identifier,
                                 std::string& port_uuid);


void send_update_event(const std::string& parent_uuid,
                       const std::string& uuid);

void set_port_neighbor_mac(const std::string& port_name,
                           const std::string& neighbor_mac);

/*!
 * @brief Determine whether a port is one of the physical ports (Ethernet X/1) or the virtual ones (Ethernet X/2, X/3, X/4) with cable in (up)
 * @param port_identifier, e.g. Ethernet X/Y
 * @param switch_config switch configuration in json format, returned from get switch running configuration command
 * @return true when is one of the physical ports (Ethernet X/1) or the virtual ones (Ethernet X/2, X/3, X/4) with cable in,
 * false when is one of the virtual ports (Ethernet X/2, X/3, X/4) without cable inserted
 */
bool is_port_physical_or_up(const std::string& port_identifier, const json::Json& switch_config);

}
}
}
