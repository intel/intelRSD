/*!
 * @brief Network interface builder class implementation.
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
 * @file network_interface_builder.cpp
 */

#include "discovery/builders/network_interface_builder.hpp"

#include <bitset>



using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;


std::string NetworkInterfaceBuilder::format_mac_address(uint8_t const* mac_as_byte_array) {
    static constexpr size_t MAC_ADDRESS_LENGTH = 18;
    char mac_address[MAC_ADDRESS_LENGTH];
    std::snprintf(&mac_address[0], MAC_ADDRESS_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
                  static_cast<unsigned int>(mac_as_byte_array[0]),
                  static_cast<unsigned int>(mac_as_byte_array[1]),
                  static_cast<unsigned int>(mac_as_byte_array[2]),
                  static_cast<unsigned int>(mac_as_byte_array[3]),
                  static_cast<unsigned int>(mac_as_byte_array[4]),
                  static_cast<unsigned int>(mac_as_byte_array[5]));

    mac_address[MAC_ADDRESS_LENGTH - 1] = '\0';
    return std::string(mac_address);
}


bool NetworkInterfaceBuilder::is_valid_mac(uint8_t const* mac_as_byte_array) {
    static constexpr int MAC_OUI_LEN = 3;
    for (int i = 0; i < MAC_OUI_LEN; ++i) {
        if (0 != mac_as_byte_array[i]) {
            return true;
        }
    }
    return false;
}


OptionalField<enums::TransportProtocol>
NetworkInterfaceBuilder::get_supported_protocol(uint16_t vendor_id, uint16_t device_id) {
    static constexpr uint16_t MELLANOX = 0x15b3;
    static constexpr uint16_t CONNECTX_4 = 0x1015;
    if (vendor_id == MELLANOX && device_id == CONNECTX_4) {
        return enums::TransportProtocol::RoCEv2;
    }
    return OptionalField<enums::TransportProtocol>{};
}


NetworkInterface NetworkInterfaceBuilder::build_default(const std::string& parent_uuid) {
    return NetworkInterface{parent_uuid};
}

