/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file eos_agent.cpp
 *
 * @brief Eos agent
 * */

#include "hal/eos_sdk/eos_agent.hpp"
#include "logger/logger_factory.hpp"
#include "eos/eth_phy_intf.h"
#include "eos/types/eth_phy_intf.h"
#include "agent-framework/exceptions/exception.hpp"
#include "utils/port.hpp"

#include <chrono>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace eos;
using namespace agent::network::hal;
using namespace agent_framework::exceptions;

namespace {
    sdk _sdk{};
    unordered_map<std::string, intf_id_t> _port_name_map{};

    void store_port_id(const std::string& name, const intf_id_t& id) {
        _port_name_map[name] = id;
    }

    const intf_id_t& get_port_id(const std::string& name) {
        return _port_name_map[name];
    }

    uint32_t eth_link_speed_to_mbps(eth_link_speed_t speed) {
        switch(speed) {
            case LINK_SPEED_10MBPS:
                return 10;
            case LINK_SPEED_100MBPS:
                return 100;
            case LINK_SPEED_1GBPS:
                return 1000;
            case LINK_SPEED_10GBPS:
                return 10000;
            case LINK_SPEED_25GBPS:
                return 25000;
            case LINK_SPEED_50GBPS:
                return 50000;
            case LINK_SPEED_40GBPS:
                return 40000;
            case LINK_SPEED_100GBPS:
                return 100000;
            case LINK_SPEED_UNKNOWN:
            case LINK_SPEED_UNEXPECTED:
            default:
                return 0;
        }
    }

    void main_loop () {
        char agent_name[] = "psme-network";
        char* argv[1] = {agent_name};
        _sdk.main_loop(1, argv);
    }
}

constexpr int EosAgent::TIMEOUT_SEC;

EosAgent::EosAgent() : agent_handler(::_sdk.get_agent_mgr()),
                       intf_handler(::_sdk.get_intf_mgr()),
                       eth_intf_handler(::_sdk.get_eth_intf_mgr()),
                       eth_phy_intf_handler(::_sdk.get_eth_phy_intf_mgr()),
                       mac_table_handler(::_sdk.get_mac_table_mgr()) {}

EosAgent::~EosAgent() {}

void EosAgent::on_initialized() {
    log_debug("network-agent", "Eos Agent initialized");
    m_initialized = true;
    m_cv.notify_all();
}

void EosAgent::start() {
    if (!m_started) {
        thread t{::main_loop};
        m_thread = move(t);
        m_started = true;
    }
}

void EosAgent::wait_until_initialized() {
    log_debug("network-agent",
              "Waiting for Eos Agent initialization...");
    unique_lock<mutex> lock(m_mutex);
    if (cv_status::timeout == m_cv.wait_for(lock,
                                            chrono::seconds(TIMEOUT_SEC))) {
        log_error("network-agent",
                  "Eos Agent initialization timeout");
        abort();
    }
    log_info("network-agent",
             "Finished waiting for Eos Agent initialization");
    watch_all_mac_entries(true);
}

void EosAgent::stop() {
    if (m_started) {
        if (m_initialized) {
            sdk_scoped_lock ssl;
            auto* agent_mgr = ::_sdk.get_agent_mgr();;
            if (agent_mgr) {
                agent_mgr->exit();
            }
            else {
                log_error("network-agent",
                          "get_agent_mgr returned NULL, EOS SDK not initialized?");
            }
        }
        m_thread.join();
        m_started = false;
    }
}

vector<std::string> EosAgent::get_port_list() const {
    vector<std::string> ret{};
    if (m_initialized) {
        sdk_scoped_lock ssl;
        auto* mgr = get_intf_mgr();
        for(auto i = mgr->intf_iter(); i; ++i){
            if (INTF_TYPE_ETH == i->intf_type() && mgr->exists(*i)) {
                ret.push_back(i->to_string());
                ::store_port_id(i->to_string(), *i);
            }
        }
    }
    else {
        log_error("network-agent",
                  "Eos Agent not yet initialized");
    }
    return ret;
}

void EosAgent:: discover_port_neighbor_mac_addr() const {
    if (m_initialized) {
        sdk_scoped_lock ssl;
        auto* mgr = get_mac_table_mgr();
        for(auto i = mgr->mac_table_status_iter(); i; ++i){
            auto entry = mgr->mac_entry_status(*i);
            agent::network::utils::set_port_neighbor_mac(
                    entry.intf().to_string(), entry.eth_addr().to_string());
            log_debug("network-agent",
                      "Discovered MAC address " << entry.eth_addr() << " on interface " << entry.intf());
        }
    }
    else {
        log_error("network-agent",
                  "Eos Agent not yet initialized");
    }
}

bool EosAgent::is_admin_enabled(const std::string& intf_name) const {
    sdk_scoped_lock ssl;
    auto* mgr = get_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    return mgr->exists(intf_id) && mgr->admin_enabled(intf_id);
}

bool EosAgent::is_operational_up(const std::string& intf_name) const {
    sdk_scoped_lock ssl;
    auto* mgr = get_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    return mgr->exists(intf_id) &&
        (eos::INTF_OPER_UP == mgr->oper_status(intf_id));
}

void EosAgent::set_admin_enable(const std::string& intf_name, const bool enable) {
    sdk_scoped_lock ssl;
    auto* mgr = get_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    try {
        mgr->admin_enabled_is(intf_id, enable);
    }
    catch (std::exception& e) {
        THROW(NetworkError, "network-agent",
              std::string("Setting administrative state on port ") + intf_name +
              " failed: " + e.what());
    }
}

uint32_t EosAgent::get_speed(const std::string& intf_name) {
    sdk_scoped_lock ssl;
    auto* mgr = get_eth_phy_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    return ::eth_link_speed_to_mbps(mgr->link_speed(intf_id));
}

void EosAgent::on_mac_entry_set(eos::mac_entry_t const &entry) {
    agent::network::utils::set_port_neighbor_mac(
            entry.intf().to_string(), entry.eth_addr().to_string());
    log_debug("network-agent",
              "Added MAC address " << entry.eth_addr() << " to interface " << entry.intf());
}

vector<uint16_t> EosAgent::get_tagged_vlans(const std::string& intf_name) {
    sdk_scoped_lock ssl;
    vector<uint16_t> vlans{};
    auto* mgr = get_eth_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    if (SWITCHPORT_MODE_TRUNK != mgr->switchport_mode(intf_id)) {
        // tagged vlans are only in trunk mode, return empty vector
        return vlans;
    }
    auto vlan_bitset = mgr->trunk_vlans(intf_id);
    if (!vlan_bitset.any()) {
        // no vlans are set
        return vlans;
    }
    // According to EOS SDK VLANs 0 and 4095 are reserved.
    // Check VLANs 1-4094, please note that number 4094 is hardcoded
    // in the SDK and there is no definition/macro for that number,
    // it will not change tough.
    for (uint16_t i = 1; i < vlan_bitset.size() && i <= 4094; i++) {
        // add tagged VLAN only if it is not a default VLAN
        // the default VLAN will be discovered as untagged VLAN
        if (vlan_bitset[i] && (i != mgr->default_vlan(intf_id))) {
            vlans.push_back(i);
        }
    }
    return vlans;
}

uint16_t EosAgent::get_default_vlan(const std::string& intf_name) {
    sdk_scoped_lock ssl;
    return get_eth_intf_mgr()->default_vlan(::get_port_id(intf_name));
}

void EosAgent::set_default_vlan(const std::string& intf_name, uint16_t vid) {
    sdk_scoped_lock ssl;
    auto intf_id = ::get_port_id(intf_name);
    auto* mgr = get_eth_intf_mgr();
    auto old_default_vlan = mgr->default_vlan(intf_id);

    try {
        // set the new default VLAN, no matter what port mode is currently
        mgr->default_vlan_is(intf_id, vid);
    }
    catch (std::exception& e) {
        THROW(NetworkError, "network-agent",
              std::string("Setting default VLAN to ") + to_string(int(vid)) +
              " on port " + intf_name + " failed: " + e.what());
    }
    // if in trunk mode we need to update the allowed VLAN list
    if (SWITCHPORT_MODE_TRUNK == mgr->switchport_mode(intf_id)) {
        try {
            mgr->trunk_vlan_del(intf_id, old_default_vlan);
        }
        catch (std::exception& e) {
            log_error("network-agent",
                      std::string("Unable to remove VLAN ") + to_string(int(old_default_vlan)) +
                      " from trunking port " + intf_name + ": " + e.what());
        }
        try {
            mgr->trunk_vlan_set(intf_id, vid);
        }
        catch (std::exception& e) {
            log_error("network-agent",
                      std::string("Unable to add VLAN ") + to_string(int(vid)) +
                      " to trunking port " + intf_name + ": " + e.what());
        }
    }
}

void EosAgent::add_tagged_vlan(const string& intf_name, uint16_t vlan_id) {
    sdk_scoped_lock ssl;
    auto* mgr = get_eth_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    if (SWITCHPORT_MODE_TRUNK == mgr->switchport_mode(intf_id)) {
        try {
            mgr->trunk_vlan_set(intf_id, vlan_id);
        }
        catch (std::exception& e) {
            log_error("network-agent",
                      std::string("Unable to add VLAN ") + to_string(int(vlan_id)) +
                      " to trunking port " + intf_name + ": " + e.what());
        }
    }
    else {
        THROW(UnsupportedValue, "network-agent",
              "Adding tagged VLAN " + to_string(int(vlan_id)) +
              " on a non-trunk port " + intf_name + " is not supported");
    }
}

void EosAgent::delete_tagged_vlan(const std::string& intf_name, uint16_t vlan_id) {
    sdk_scoped_lock ssl;
    auto* mgr = get_eth_intf_mgr();
    auto intf_id = ::get_port_id(intf_name);
    if (SWITCHPORT_MODE_TRUNK == mgr->switchport_mode(intf_id)) {
        try {
            mgr->trunk_vlan_del(intf_id, vlan_id);
        }
        catch (std::exception& e) {
            log_error("network-agent",
                      std::string("Unable to delete VLAN ") + to_string(int(vlan_id)) +
                      " from trunking port " + intf_name + ": " + e.what());
        }
    }
    else {
        THROW(UnsupportedValue, "network-agent",
              "Deleting tagged VLAN " + to_string(int(vlan_id)) +
              " on a non-trunk port " + intf_name + " is not supported");
    }
}
