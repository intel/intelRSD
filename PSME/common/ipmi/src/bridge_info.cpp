/*!
 * @brief Class to keep ipmi bridging information
 *
 * Simple container, not to be overriden.
 *
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
 * @file ipmi/ipmi_bridge.cpp
 */

#include "ipmi/bridge_info.hpp"

#include <stdexcept>

namespace {

void address_channel(std::ostream& ostr, const ipmi::BridgeInfo& info, bool target) {
    ipmi::BridgeInfo::Address address;
    ipmi::BridgeInfo::Channel channel;
    if (target) {
        info.get_target(address, channel);
    } else {
        info.get_transit(address, channel);
    }
    ostr << std::hex << address << "." << std::hex << static_cast<unsigned>(channel);
}

}

std::ostream& operator<<(std::ostream& ostr, const ipmi::BridgeInfo& info) {

    switch (info.get_level()) {
        case ipmi::BridgeInfo::Level::DIRECT:
            break;
        case ipmi::BridgeInfo::Level::SINGLE_BRIDGE:
            ostr << ":";
            address_channel(ostr, info, true);
            break;
        case ipmi::BridgeInfo::Level::DUAL_BRIDGE:
            ostr << ":";
            address_channel(ostr, info, false);
            ostr << ":";
            address_channel(ostr, info, true);
            break;
        default:
            /* assert.. */
            throw std::runtime_error("Illegal bridging level");

    }
    return ostr;
}


using namespace ipmi;

BridgeInfo::BridgeInfo(): level(Level::DIRECT) { }

BridgeInfo::BridgeInfo(BridgeInfo::Address  _target_address, BridgeInfo::Channel _target_channel) :
    level(Level::SINGLE_BRIDGE),
    target_channel(_target_channel),
    target_address(_target_address) { }

BridgeInfo::BridgeInfo(BridgeInfo::Address _transit_address, BridgeInfo::Channel _transit_channel,
                       BridgeInfo::Address _target_address, BridgeInfo::Channel _target_channel) :
    level(Level::DUAL_BRIDGE),
    transit_channel(_transit_channel),
    transit_address(_transit_address),
    target_channel(_target_channel),
    target_address(_target_address) { }

BridgeInfo::~BridgeInfo() { }

bool BridgeInfo::get_target(BridgeInfo::Address& address, BridgeInfo::Channel& channel) const {
    if (level == Level::DIRECT) {
        return false;
    }
    address = target_address;
    channel = target_channel;
    return true;
}

bool BridgeInfo::set_target(BridgeInfo::Address address, BridgeInfo::Channel channel) {
    if (level == Level::DIRECT) {
        return false;
    }
    target_address = address;
    target_channel = channel;
    return true;
}

bool BridgeInfo::set_target(BridgeInfo::Channel channel) {
    if (level == Level::DIRECT) {
        return false;
    }
    target_channel = channel;
    return true;
}

bool BridgeInfo::get_transit(BridgeInfo::Address& address, BridgeInfo::Channel& channel) const {
    if (level != Level::DUAL_BRIDGE) {
        return false;
    }
    address = transit_address;
    channel = transit_channel;
    return true;
}

bool BridgeInfo::set_transit(BridgeInfo::Address address, BridgeInfo::Channel channel) {
    if (level != Level::DUAL_BRIDGE) {
        return false;
    }
    transit_address = address;
    transit_channel = channel;
    return true;
}

bool BridgeInfo::set_transit(BridgeInfo::Channel channel) {
    if (level != Level::DUAL_BRIDGE) {
        return false;
    }
    transit_channel = channel;
    return true;
}
