/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module/network_interface.hpp"
#include "json/json.hpp"

using namespace agent_framework::generic;

void NetworkInterface::read_configuration(const json::Value& interface_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading network interface configuration.");

        m_frame_size = interface_configuration["frameSize"].as_uint();
        m_link_technology = interface_configuration["linkTechnology"].as_string();
        m_speed_gbps = interface_configuration["speedGbps"].as_uint();
        m_autosense = interface_configuration["autosense"].as_bool();
        m_mac_address = interface_configuration["macAddress"].as_string();
        m_vlan_enable = interface_configuration["vlanEnable"].as_bool();
        m_vlan_count = interface_configuration["vlanCount"].as_uint();

        auto& ipv4_address = interface_configuration["ipv4Address"];
        m_ipv4_address.set_address(ipv4_address["address"].as_string());
        m_ipv4_address.set_subnet_mask(ipv4_address["subnetMask"].as_string());
        m_ipv4_address.set_address_origin(
            ipv4_address["addressOrigin"].as_string());
        m_ipv4_address.set_gateway(ipv4_address["gateway"].as_string());

        auto& ipv6_address = interface_configuration["ipv6Address"];
        m_ipv6_address.set_address(ipv6_address["address"].as_string());
        m_ipv6_address.set_prefix_length(
            ipv6_address["prefixLength"].as_uint());
        m_ipv6_address.set_address_origin(
            ipv6_address["addressOrigin"].as_string());
        m_ipv6_address.set_address_state(
            ipv6_address["addressState"].as_string());

        auto& neighbor_info = interface_configuration["neighborInfo"];
        m_neighbor_info.set_switch_identifier(
            neighbor_info["switchIdentifier"].as_string());
        m_neighbor_info.set_port_identifier(
            neighbor_info["portIdentifier"].as_string());

        set_status({interface_configuration["status"]["state"].as_string(),
                    interface_configuration["status"]["health"].as_string()});
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing network interface configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in networkInterfaces section");
    }
}


NetworkInterface::~NetworkInterface(){}

