/*!
 * @brief Network device function builder class implementation.
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
 * @file network_device_function_builder.cpp
 */

#include "utils/compute_conversions.hpp"
#include "discovery/builders/network_device_function_builder.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "mdr/printers.hpp"



using namespace agent_framework::model;

namespace agent {
namespace compute {
namespace discovery {

agent_framework::model::NetworkDeviceFunction NetworkDeviceFunctionBuilder::build_default(
    const std::string& parent_uuid) {
    agent_framework::model::NetworkDeviceFunction ndf{parent_uuid};
    ndf.set_status({enums::State::Disabled, {}});
    return ndf;
}


void NetworkDeviceFunctionBuilder::update_iscsi_mdr_data(agent_framework::model::NetworkDeviceFunction& ndf,
                                                         const mdr::StructEnhanced<iscsi::structs::ISCSI_MDR_INITIATOR>& initiator,
                                                         const mdr::StructEnhanced<iscsi::structs::ISCSI_MDR_TARGET>& target) {
    attribute::IscsiBoot boot;
    ndf.set_mac_address(mdr::print_mac(initiator.data.nic_mac_address));
    boot.set_authentication_method(
        agent::compute::utils::IscsiMdrConverter::get_authentication_method(
            target.data.authentication_method));
    boot.set_chap_secret(target.get_string(target.data.chap_secret));
    boot.set_chap_username(target.get_string(target.data.chap_user_name));
    boot.set_initiator_address(mdr::print_ip(initiator.data.initiator_ip_address));
    boot.set_initiator_default_gateway(mdr::print_ip(initiator.data.initiator_gateway));
    boot.set_initiator_name(initiator.get_string(initiator.data.initiator_name));
    boot.set_initiator_netmask(mdr::print_ip(initiator.data.initiator_subnet_mask));
    boot.set_ip_address_type(
        agent::compute::utils::IscsiMdrConverter::get_ip_type(target.data.ip_address_type));
    boot.set_ip_mask_dns_via_dhcp(static_cast<bool>(initiator.data.initiator_dhcp_enabled));
    boot.set_mutual_chap_secret(target.get_string(target.data.mutual_chap_secret));
    boot.set_mutual_chap_username(target.get_string(target.data.mutual_chap_name));
    boot.set_primary_dns(mdr::print_ip(initiator.data.initiator_primary_dns));
    boot.set_primary_lun(target.data.target_lun);
    boot.set_primary_target_address(mdr::print_ip(target.data.target_ip_address));
    boot.set_primary_target_name(target.get_string(target.data.target_name));
    boot.set_primary_target_port(target.data.target_port);
    boot.set_primary_vlan_enable(static_cast<bool>(target.data.vlan_enabled));
    if (target.data.vlan_id >= 1 && target.data.vlan_id <= 4094) {
        boot.set_primary_vlan_id(target.data.vlan_id);
    }
    boot.set_router_advertisement_enabled(static_cast<bool>(target.data.router_advertisement));
    boot.set_secondary_dns(mdr::print_ip(initiator.data.initiator_secondary_dns));
    boot.set_target_info_via_dhcp(static_cast<bool>(target.data.target_dhcp_enabled));
    // the secondary fields are not used

    ndf.set_iscsi_boot(boot);
}


void NetworkDeviceFunctionBuilder::clear_iscsi_mdr_data(agent_framework::model::NetworkDeviceFunction& ndf) {
    using namespace agent_framework::model;
    attribute::IscsiBoot boot;
    ndf.set_mac_address(OptionalField<std::string>());

    ndf.set_iscsi_boot(boot);
}

}  // namespace discovery_ref
}  // namespace compute
}  // namespace agent
