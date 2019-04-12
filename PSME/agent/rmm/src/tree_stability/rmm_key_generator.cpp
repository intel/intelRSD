/*!
 * @brief RMMKeyGenerator class implementation.
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
 * @file rmm_key_generator.cpp
 */

#include "tree_stability/rmm_key_generator.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include <numeric>

using namespace agent_framework::module;
using namespace agent_framework::model;

namespace agent {
namespace rmm {

using map_value_type = std::map<std::string, std::string>::value_type;
using agent_framework::KeyValueMissingError;

std::string RmmKeyGenerator::m_agent_id{};

const std::map<std::string, std::string> RmmKeyGenerator::m_keys_base_map{
    map_value_type(Manager::get_component().to_string(), "_RMMManager_"),
    map_value_type(Chassis::get_component().to_string(), "_RMMChassis_"),
    map_value_type(NetworkInterface::get_component().to_string(), "_RMMNetworkInterface_"),
    map_value_type(Fan::get_component().to_string(), "_RMMFan_"),
    map_value_type(EthernetSwitchPortVlan::get_component().to_string(), "_RMMVlan_"),
    map_value_type(PowerZone::get_component().to_string(), "_RMMPowerZone_"),
    map_value_type(Psu::get_component().to_string(), "_RMMPsu_"),
    map_value_type(ThermalZone::get_component().to_string(), "_RMMThermalZone_")
};


/* A Chassis of type Rack or Drawer can be assigned a unique property on their own */
template<>
const std::string RmmKeyGenerator::generate_key(const Chassis& chassis) {
    const enums::ChassisType chassis_type = chassis.get_type();
    OptionalField<std::string> unique_property{};

    if (enums::ChassisType::Rack == chassis_type) {
        unique_property = chassis_type.to_string();
    }
    else if (enums::ChassisType::Drawer == chassis_type) {
        unique_property = chassis.get_fru_info().get_serial_number();
    }
    else {
        throw std::logic_error("Improper usage of RMM key generator.");
    }

    if (!unique_property.has_value()) {
        throw KeyValueMissingError(
            "Unique property for chassis of type " + std::string(chassis_type.to_string()) + " is missing");
    }

    return generate_key_base(chassis) + unique_property.value();
}


/* A Chassis of type Zone needs to have zone NIC provided */
template<>
const std::string RmmKeyGenerator::generate_key(const Chassis& chassis, const NetworkInterface& nic) {
    const enums::ChassisType chassis_type = chassis.get_type();
    OptionalField<std::string> unique_property{};

    if (enums::ChassisType::Zone == chassis_type) {
        unique_property = nic.get_mac_address();
    }
    else {
        throw std::logic_error("Improper usage of RMM key generator.");
    }

    if (!unique_property.has_value()) {
        throw KeyValueMissingError(
            "Unique property for chassis of type " + std::string(chassis_type.to_string()) + " is missing");
    }

    return generate_key_base(chassis) + unique_property.value();
}


/* A NetworkInterface type resource can be assigned a unique property on their own */
template<>
const std::string RmmKeyGenerator::generate_key(const NetworkInterface& interface) {
    const auto mac_address = interface.get_mac_address();

    if (!mac_address.has_value()) {
        throw KeyValueMissingError("MAC address is missing.");
    }

    return generate_key_base(interface) + mac_address.value();
}


/* A Fan needs to have its ancestor Zone NIC provided */
template<>
const std::string RmmKeyGenerator::generate_key(const Fan& fan, const NetworkInterface& zone_network_interface) {
    const auto zone_nic_mac = zone_network_interface.get_mac_address();
    const auto slot_id = fan.get_slot_id();

    if (!zone_nic_mac.has_value()) {
        throw KeyValueMissingError("Zone NIC MAC address is missing.");
    }

    return generate_key_base(fan) + zone_nic_mac.value() + std::to_string(static_cast<unsigned int>(slot_id));
}


/* A Manager needs to have its managed resource NICs provided if there are multiple NICs */
template<>
const std::string RmmKeyGenerator::generate_key(const Manager& manager, const std::vector<NetworkInterface>& network_interfaces) {
    std::vector<std::string> macs{};

    for (const auto& nic : network_interfaces) {
        if (nic.get_mac_address().has_value()) {
            macs.push_back(nic.get_mac_address());
        }
    }

    if (macs.empty()) {
        throw KeyValueMissingError("NICs MAC addresses are missing.");
    }

    std::sort(macs.begin(), macs.end());
    std::string key = std::accumulate(macs.begin(), macs.end(), std::string());

    return generate_key_base(manager) + key;
}


/* An EthernetSwitchPortVlan needs to have its NIC provided */
template<>
const std::string
RmmKeyGenerator::generate_key(const EthernetSwitchPortVlan& vlan, const NetworkInterface& network_interface) {
    const auto vlan_id = vlan.get_vlan_id();
    const auto nic_mac = network_interface.get_mac_address();

    if ((!vlan_id.has_value()) || (!nic_mac.has_value())) {
        throw KeyValueMissingError("Unique propery not found.");
    }

    return generate_key_base(vlan) + std::to_string(vlan_id.value()) + nic_mac.value();
}


/* A PowerZone needs to have its ancestor Zone NIC provided */
template<>
const std::string
RmmKeyGenerator::generate_key(const PowerZone& power_zone, const NetworkInterface& zone_network_interface) {
    const auto zone_nic_mac = zone_network_interface.get_mac_address();

    if (!zone_nic_mac.has_value()) {
        throw KeyValueMissingError("Zone NIC MAC address is missing.");
    }

    return generate_key_base(power_zone) + zone_nic_mac.value();
}


/* A ThermalZone needs to have its ancestor Zone NIC provided */
template<>
const std::string
RmmKeyGenerator::generate_key(const ThermalZone& thermal_zone, const NetworkInterface& zone_network_interface) {
    const auto zone_nic_mac = zone_network_interface.get_mac_address();

    if (!zone_nic_mac.has_value()) {
        throw KeyValueMissingError("Zone NIC MAC address is missing.");
    }

    return generate_key_base(thermal_zone) + zone_nic_mac.value();
}


/* A Psu type resource can be assigned a unique property on their own */
template<>
const std::string RmmKeyGenerator::generate_key(const Psu& psu) {
    const auto serial_number = psu.get_fru_info().get_serial_number();

    if (!serial_number.has_value()) {
        throw KeyValueMissingError("Serial number is missing.");
    }

    return generate_key_base(psu) + serial_number.value();
}


template<>
const std::string RmmKeyGenerator::generate_key(const Metric& metric, const Resource& resource) {
    const auto& resource_key = resource.get_unique_key();

    return resource_key + metric.get_component().to_string() + metric.get_name();
}


template<>
const std::string RmmKeyGenerator::generate_key(const MetricDefinition& metric_def, const std::string& name) {
    return metric_def.get_component().to_string() + name;
}

}
}
