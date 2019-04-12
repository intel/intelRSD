/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "loader/network_config.hpp"
#include <algorithm>

using namespace std;
using namespace agent::network::loader;

void NetworkConfig::add_port_ethmode(const string& port_identifier,
    const EthMode mode) {
    lock_guard<mutex> lock{m_mutex};
    m_port_eth_mode_map.emplace(port_identifier, mode);
}

const EthMode NetworkConfig::get_port_ethmode(const string& port_identifier) {
    lock_guard<mutex> lock{m_mutex};
    auto it = find_if(m_port_eth_mode_map.cbegin(), m_port_eth_mode_map.cend(),
                        [&port_identifier](const pair<string, EthMode>& p) {
                            return port_identifier == p.first;
                        });
    if (m_port_eth_mode_map.cend() != it) {
        return it->second;
    }
    return EthMode::Default;
}

void NetworkConfig::add_port_autoneg(const string& port_identifier,
    const AutoNeg autoneg) {
    lock_guard<mutex> lock{m_mutex};
    m_port_autoneg_map.emplace(port_identifier, autoneg);
}

const AutoNeg NetworkConfig::get_port_autoneg(const string& port_identifier) {
    lock_guard<mutex> lock{m_mutex};
    auto it = find_if(m_port_autoneg_map.cbegin(), m_port_autoneg_map.cend(),
                        [&port_identifier](const pair<string, AutoNeg>& p) {
                            return port_identifier == p.first;
                        });
    if (m_port_autoneg_map.cend() != it) {
        return it->second;
    }
    return AutoNeg::Default;
}

void NetworkConfig::add_switch_mgmt_port(const string& switch_identifier,
    const string& mgmt_port) {
    lock_guard<mutex> lock{m_mutex};
    m_switch_mgmt_map.emplace(switch_identifier, mgmt_port);
}

const string& NetworkConfig::get_switch_mgmt_port(
                            const string& switch_identifier) {
    lock_guard<mutex> lock{m_mutex};
    auto it = m_switch_mgmt_map.find(switch_identifier);
    if (m_switch_mgmt_map.end() != it) {
        return it->second;
    }
    return DEFAULT_MGMT_PORT;
}

void NetworkConfig::add_public_vlan(uint32_t vlan_id) {
    lock_guard<mutex> lock{m_mutex};
    m_public_vlans.emplace_back(vlan_id);
}

std::vector<uint32_t> NetworkConfig::get_public_vlans() {
    lock_guard<mutex> lock{m_mutex};
    return m_public_vlans;
}

void NetworkConfig::add_acl_name(const string& uuid, const string& name) {
    lock_guard<mutex> lock{m_mutex};
    m_acl_map[uuid] = name;
}

const string& NetworkConfig::get_acl_name (const string& uuid) {
    lock_guard<mutex> lock{m_mutex};
    if (m_acl_map.find(uuid) != m_acl_map.end()) {
        return m_acl_map[uuid];
    }
    return uuid;
}

NetworkConfig::~NetworkConfig() { }
