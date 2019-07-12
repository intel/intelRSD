/*!
 * @brief Implementation of NvmeKeyGenerator class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file nvme_key_generator.cpp
 */


#include "tree_stability/nvme_key_generator.hpp"
#include "loader/config.hpp"



using namespace agent::nvme::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;

namespace agent {
namespace nvme {

const std::map<std::string, std::string> NvmeKeyGenerator::m_keys_base_map{
    {Manager::get_component().to_string(),          "_NvmeManager_"},
    {Chassis::get_component().to_string(),          "_NvmeChassis_"},
    {Drive::get_component().to_string(),            "_NvmeDrive_"},
    {Volume::get_component().to_string(),           "_NvmeVolume_"},
    {Fabric::get_component().to_string(),           "_NvmeFabric_"},
    {Endpoint::get_component().to_string(),         "_NvmeEndpoint_"},
    {System::get_component().to_string(),           "_NvmeSystem_"},
    {NetworkInterface::get_component().to_string(), "_NvmeNetworkInterface_"},
    {StorageService::get_component().to_string(),   "_NvmeStorageService_"},
    {StoragePool::get_component().to_string(),      "_NvmeStoragePool_"}
};

std::string NvmeKeyGenerator::m_agent_id{};


std::string NvmeKeyGenerator::get_unique_key(const Manager& manager) {
    return manager.get_ipv4_address();
}


std::string NvmeKeyGenerator::get_unique_key(const Chassis&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


std::string NvmeKeyGenerator::get_unique_key(const Fabric&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


std::string NvmeKeyGenerator::get_unique_key(const System&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


std::string NvmeKeyGenerator::get_unique_key(const StorageService&) {
    return NvmeConfig::get_instance()->get_parent_id();
}


std::string NvmeKeyGenerator::get_unique_key(const NetworkInterface& interface) {
    const auto& mac = interface.get_mac_address();
    if (mac.has_value()) {
        return mac.value();
    }
    else {
        throw KeyValueMissingError("Mac has no value.");
    }
}


std::string NvmeKeyGenerator::get_unique_key(const Drive& drive) {
    std::string model{};
    std::string serial{};

    if (drive.get_fru_info().get_model_number().has_value()) {
        model = drive.get_fru_info().get_model_number();
    }
    else {
        throw KeyValueMissingError("Drive '" + drive.get_uuid() + "' has no model number.");
    }

    if (drive.get_fru_info().get_serial_number().has_value()) {
        serial = drive.get_fru_info().get_serial_number();
    }
    else {
        throw KeyValueMissingError("Drive '" + drive.get_uuid() + "' has no serial number.");
    }
    return model + serial;
}


std::string NvmeKeyGenerator::get_unique_key(const Volume& volume) {
    std::string uuid{};
    try {
        uuid = attribute::Identifier::get_uuid(volume);
    }
    catch (const std::logic_error&) {
        throw KeyValueMissingError("Volume '" + volume.get_uuid() + "' has no 'by-partuuid'.");
    }

    if (uuid.empty()) {
        throw KeyValueMissingError("Volume '" + volume.get_uuid() + "' has empty 'by-partuuid'.");
    }

    return uuid;
}


std::string NvmeKeyGenerator::get_unique_key(const StoragePool& storage_pool) {

    if (storage_pool.get_capacity_sources().empty()) {
        throw KeyValueMissingError("StoragePool '" + storage_pool.get_uuid() +
                                   "' has no underlying capacity sources.");
    }

    const auto& providing_drives = storage_pool.get_capacity_sources().front().get_providing_drives();
    if (providing_drives.empty()) {
        throw KeyValueMissingError("StoragePool '" + storage_pool.get_uuid() +
                                   "' has no underlying drives in the capacity sources.");
    }

    auto uuid = providing_drives.front();
    return uuid;
}


std::string NvmeKeyGenerator::get_unique_key(const Endpoint& endpoint) {
    std::string key{};
    try {
        key = attribute::Identifier::get_nqn(endpoint);
    }
    catch (const std::logic_error&) {
        throw KeyValueMissingError("Endpoint '" + endpoint.get_uuid() + "' has no NQN.");
    }

    return key;
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
