/*!
 * @brief Implementation of NvmeKeyGenerator class.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file nvme_key_generator.cpp
 */


#include "tree_stability/nvme_key_generator.hpp"
#include "loader/config.hpp"



using namespace agent::nvme::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace std;

namespace {

template<typename T>
string get_system_path_identifier(const T& resource) {
    try {
        return attribute::Identifier::get_system_path(resource);
    }
    catch (const logic_error&) {
        return "";
    }
}

}

namespace agent {
namespace nvme {

const map<string, string> NvmeKeyGenerator::m_keys_base_map{
    {Manager::get_component().to_string(), "_NvmeManager_"},
    {Chassis::get_component().to_string(), "_NvmeChassis_"},
    {Drive::get_component().to_string(), "_NvmeDrive_"},
    {Volume::get_component().to_string(), "_NvmeVolume_"},
    {Fabric::get_component().to_string(), "_NvmeFabric_"},
    {Endpoint::get_component().to_string(), "_NvmeEndpoint_"},
    {System::get_component().to_string(), "_NvmeSystem_"},
    {NetworkInterface::get_component().to_string(), "_NvmeNetworkInterface_"},
    {StorageService::get_component().to_string(), "_NvmeStorageService_"},
    {StoragePool::get_component().to_string(), "_NvmeStoragePool_"}
};

string NvmeKeyGenerator::m_agent_id{};


string NvmeKeyGenerator::get_unique_key(const Manager& manager) {
    return manager.get_ipv4_address() +
           (NvmeConfig::get_instance()->get_is_target() ? "_target" : "_initiator");
}


string NvmeKeyGenerator::get_unique_key(const Chassis&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


string NvmeKeyGenerator::get_unique_key(const Fabric&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


string NvmeKeyGenerator::get_unique_key(const System&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


string NvmeKeyGenerator::get_unique_key(const StorageService&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


string NvmeKeyGenerator::get_unique_key(const NetworkInterface& interface) {
    const auto& mac = interface.get_mac_address();
    if (mac.has_value()) {
        return mac.value();
    }
    return "";
}


string NvmeKeyGenerator::get_unique_key(const Drive& drive) {
    return get_system_path_identifier(drive);
}


string NvmeKeyGenerator::get_unique_key(const Volume& volume) {
    return get_system_path_identifier(volume);
}


string NvmeKeyGenerator::get_unique_key(const StoragePool& storage_pool) {
    return get_system_path_identifier(storage_pool);
}

string NvmeKeyGenerator::get_unique_key(const Endpoint& endpoint) {
    string ret{};
    try {
        ret = attribute::Identifier::get_nqn(endpoint);
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
std::string NvmeKeyGenerator::generate_key(const Metric& metric, const Resource& resource) {
    const auto& resource_key = resource.get_unique_key();

    return resource_key + metric.get_component().to_string() + metric.get_metric_definition_uuid() + metric.get_name();
}

template<>
std::string NvmeKeyGenerator::generate_key(const MetricDefinition& metric_def) {
    std::string key = metric_def.get_component().to_string() + metric_def.get_metric_jsonptr();
    if (metric_def.get_name().has_value()) {
        key += metric_def.get_name();
    }
    return key;
}

}
}
