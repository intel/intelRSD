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
 *
 * @file network_interface.cpp
 * @brief SMBIOS-way of discovering network interface properties
 * */

#include "agent-framework/module/compute_components.hpp"
#include "discovery/smbios/network_interface.hpp"
#include "discovery/common.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

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

std::tuple<bool, enums::Health> discover_network_interfaces(SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::OK);
    }

    auto smbios_network_interfaces = smbios->get_all<SMBIOS_NIC_INFO_DATA>();

    int added_interface_count{0};
    for (const auto& nic_info : smbios_network_interfaces) {
        // skip interface with invalid MAC address (happens for rrc interface with BIOS in legacy mode)
        if (is_valid_mac(nic_info.data.PortData.Mac)) {
            std::string mac_address = format_mac_address(nic_info.data.PortData.Mac);
            NetworkInterface interface{system.get_uuid()};
            interface.set_mac_address(mac_address);
            interface.set_factory_mac_address(mac_address);
            interface.set_status(attribute::Status(true));

            ComputeComponents::get_instance()->get_network_interface_manager().add_entry(std::move(interface));
            log_debug(GET_LOGGER("smbios-discovery"), "Added a Network interface successfully (smbios).");
            added_interface_count++;
        }
    }

    // BIOS workaround for two cases:
    // 1. BIOS in legacy mode can not read rrc MAC address
    // 2. BIOS sporadically does not discover one rrc interface
    if (1 == added_interface_count) {
        // add interface based on netlink discovery
        // (if MAC can't be read the interface in Disabled state is added)
        if (discover_rrc_network_interface(system)) {
            log_debug(GET_LOGGER("smbios-discovery"), "Added a Network interface successfully (netlink).");
        }
        added_interface_count++;
    }

    log_info(GET_LOGGER("smbios-discovery"), "Network Interface Discovery: "
                << added_interface_count << " interface(s) added / "
                << smbios_network_interfaces.size() << " structs present.");

    return std::make_tuple((added_interface_count > 0), enums::Health::OK);
}

}
}
}
}
