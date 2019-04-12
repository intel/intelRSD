/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm/loader/ipmi_config.cpp
 */

#include "loader/ipmi_config.hpp"

using namespace agent::rmm::loader;

namespace {

constexpr uint8_t MMP_BRIDGE = 0x24;
constexpr uint8_t CPP_BRIDGE = 0x10;
constexpr uint8_t CPP_CHANNEL = 0x02;

}

IpmiConfig& IpmiConfig::get_instance() {
    static IpmiConfig config{};
    return config;
}

std::vector<IpmiConfig::LocationOffset> IpmiConfig::get_controller_slots() const {
    std::vector<IpmiConfig::LocationOffset> slots{};
    for (ControllerMap::const_iterator it = controllers.begin(); it != controllers.end(); it++) {
        slots.push_back(it->first);
    }
    return slots;
}


ipmi::IpmiController::Ptr IpmiConfig::get_controller(IpmiConfig::LocationOffset slot) {
    ControllerMap::const_iterator it = controllers.find(slot);
    if (it == controllers.end()) {
        throw std::runtime_error("Slot #" + std::to_string(static_cast<unsigned>(slot)) + " not in config");
    }
    return it->second;
}

ipmi::BridgeInfo IpmiConfig::get_cm_bridge() {
    /* direct bridge */
    return ipmi::BridgeInfo{};
}

ipmi::BridgeInfo IpmiConfig::get_mbp_bridge(std::uint8_t chassis_slot) {
    /* single bridge */
    return ipmi::BridgeInfo{::MMP_BRIDGE, chassis_slot};
}

ipmi::BridgeInfo IpmiConfig::get_psme_chassis_bridge(std::uint8_t drawer_slot) {
    /* dual bridge, via MBP */
    return ipmi::BridgeInfo{::MMP_BRIDGE, drawer_slot, ::CPP_BRIDGE, ::CPP_CHANNEL};
}

void IpmiConfig::add_controller(IpmiConfig::LocationOffset slot, ipmi::IpmiController::Ptr controller) {
    if (controller) {
        // add controller
        controllers[slot] = controller;
    } else {
        // remove controller
        ControllerMap::iterator it = controllers.find(slot);
        if (it != controllers.end()) {
            controllers.erase(it);
        }
    }
}

IpmiConfig::IpmiConfig() { }

IpmiConfig::~IpmiConfig() { }
