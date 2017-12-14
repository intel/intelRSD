/*!
 * @brief Network interface builder class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file network_interface_builder.hpp
 */

#include "discovery/builders/platform_specific/grantley/grantley_network_interface_builder.hpp"

#include <bitset>
#include <algorithm>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
static constexpr size_t MAC_ADDRESS_LENGTH = 18;


std::string format_mac_address(uint8_t const* mac_as_byte_array) {
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


bool is_valid_mac(uint8_t const* mac_as_byte_array) {
    static constexpr int MAC_OUI_LEN = 3;
    for (int i = 0; i < MAC_OUI_LEN; ++i) {
        if (0 != mac_as_byte_array[i]) {
            return true;
        }
    }
    return false;
}

}


NetworkInterface NetworkInterfaceBuilder::build_default(const std::string& parent_uuid) {
    return NetworkInterface{parent_uuid};
}


void NetworkInterfaceBuilder::update_smbios_data(agent_framework::model::NetworkInterface& network_interface,
                                                 const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA>& smbios_data) {

    if (is_valid_mac(smbios_data.data.PortData.Mac)) {
        std::string mac_address = format_mac_address(smbios_data.data.PortData.Mac);
        network_interface.set_mac_address(mac_address);
        network_interface.set_factory_mac_address(mac_address);
        network_interface.set_status({enums::State::Enabled, {}});
    }
}

void NetworkInterfaceBuilder::update_smbios_data(agent_framework::model::NetworkInterface& network_interface,
                                                 const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA_V2>& smbios_data) {

    if (is_valid_mac(smbios_data.data.PortData.Mac)) {
        std::string mac_address = format_mac_address(smbios_data.data.PortData.Mac);
        network_interface.set_mac_address(mac_address);
        network_interface.set_factory_mac_address(mac_address);
        network_interface.set_status({enums::State::Enabled, {}});
    }
}
