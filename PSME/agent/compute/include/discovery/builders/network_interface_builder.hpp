/*!
 * @brief Network interface builder class interface.
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
 * @file network_interface_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/network_interface.hpp"
#include "smbios/smbios_parser.hpp"




namespace agent {
namespace compute {
namespace discovery {

class NetworkInterfaceBuilder {

public:

    /*!
     * @brief Build default network inteface object.
     * @param parent_uuid Parent UUID.
     * @return Default build network interface object.
     */
    static agent_framework::model::NetworkInterface build_default(const std::string& parent_uuid);


    /*!
     * @brief Update network interface object with SMBIOS data.
     * @param network_interface Network interface object to be updated with SMBIOS data.
     * @param smbios_data SMBIOS data.
     */
    template<typename SmbiosNicStruct>
    static void update_smbios_data(agent_framework::model::NetworkInterface& network_interface,
                                   const SmbiosNicStruct& smbios_data) {

        if (is_valid_mac(smbios_data.data.PortData.Mac)) {
            std::string mac_address = format_mac_address(smbios_data.data.PortData.Mac);
            network_interface.set_mac_address(mac_address);
            network_interface.set_factory_mac_address(mac_address);
            network_interface.set_status({agent_framework::model::enums::State::Enabled, {}});

            agent_framework::model::attribute::Array<agent_framework::model::enums::TransportProtocol> protocols_array{};
            const auto protocol = get_supported_protocol(smbios_data.data.VendorID, smbios_data.data.DeviceID);
            if (protocol.has_value()) {
                protocols_array.add_entry(protocol);
            }
            network_interface.set_supported_transport_protocols(protocols_array);
        }
        else {
            log_warning("compute-agent", "Invalid MAC address: " << format_mac_address(smbios_data.data.PortData.Mac));
        }
    }


private:

    /*!
     * @brief Convert MAC address to string.
     * @param mac_as_byte_array MAC address array.
     * @return MAC address as a string.
     */
    static std::string format_mac_address(uint8_t const* mac_as_byte_array);


    /*!
     * @brief Check if MAC address is valid.
     * @param mac_as_byte_array MAC address array.
     * @return True if MAC is valid, false otherwise.
     */
    static bool is_valid_mac(uint8_t const* mac_as_byte_array);


    /*!
     * @brief Return supported RDMA protocol for EthernetInterface.
     * @param vendor_id Vendor ID.
     * @param device_id Device ID.
     * @return supported RDMA protocol.
     */
    static OptionalField<agent_framework::model::enums::TransportProtocol>
    get_supported_protocol(uint16_t vendor_id, uint16_t device_id);

};

}
}
}
