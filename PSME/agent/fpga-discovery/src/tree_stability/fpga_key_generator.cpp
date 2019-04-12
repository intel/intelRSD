/*!
 * @brief Implementation of FpgaKeyGenerator class.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file fpga_key_generator.cpp
 */


#include "tree_stability/fpga_key_generator.hpp"
#include "loader/config.hpp"



using namespace agent::fpga_discovery::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace std;


namespace agent {
namespace fpga_discovery {

string FpgaKeyGenerator::m_agent_id{};


string FpgaKeyGenerator::get_unique_key(const Manager& manager) {
    return manager.get_ipv4_address() + "_fpga_discovery";
}


string FpgaKeyGenerator::get_unique_key(const Chassis&) {
    return FpgaDiscoveryConfig::get_instance()->get_parent_id();
}


string FpgaKeyGenerator::get_unique_key(const Fabric&) {
    return FpgaDiscoveryConfig::get_instance()->get_parent_id();
}


string FpgaKeyGenerator::get_unique_key(const System&) {
    return FpgaDiscoveryConfig::get_instance()->get_parent_id();
}


string FpgaKeyGenerator::get_unique_key(const NetworkInterface& interface) {
    const auto& mac = interface.get_mac_address();
    if (mac.has_value()) {
        return mac.value();
    }
    return "";
}


string FpgaKeyGenerator::get_unique_key(const Endpoint& endpoint) {
    string ret{};
    try {
        ret = attribute::Identifier::get_uuid(endpoint);
    }
    catch (const std::logic_error&) {
    }
    for (const auto& ce : endpoint.get_connected_entities()) {
        const auto& role = ce.get_entity_role();
        if (role.has_value()) {
            ret += ":" + role.value();
        }
    }
    return ret;
}

template<>
std::string FpgaKeyGenerator::generate_key(const Metric& metric, const Resource& resource) {
    const auto& resource_key = resource.get_unique_key();

    return resource_key + metric.get_component().to_string() + metric.get_metric_definition_uuid() + metric.get_name();
}

template<>
std::string FpgaKeyGenerator::generate_key(const MetricDefinition& metric_def) {
    std::string key = metric_def.get_component().to_string() + metric_def.get_metric_jsonptr();
    if (metric_def.get_name().has_value()) {
        key += metric_def.get_name();
    }
    return key;
}

}
}
