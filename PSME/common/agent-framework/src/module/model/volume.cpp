/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file volume.cpp
 * @brief Volume model implementation
 * */

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/volume.hpp"
#include "agent-framework/module/constants/storage.hpp"

#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/model/zone.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Volume::component = enums::Component::Volume;
const enums::CollectionName Volume::collection_name = enums::CollectionName::Volumes;


Volume::Volume(const std::string& parent_uuid, enums::Component parent_type) :
    Resource(parent_uuid, parent_type) {}


Volume::~Volume() {}


json::Json Volume::to_json() const {
    json::Json result{};
    result[literals::Volume::NAME] = get_name();
    result[literals::Volume::DESCRIPTION] = get_description();
    result[literals::Volume::STATUS] = get_status().to_json();
    result[literals::Volume::COLLECTIONS] = get_collections().to_json();
    result[literals::Volume::BLOCK_SIZE_BYTES] = get_block_size_bytes();
    result[literals::Volume::CAPACITY] = get_capacity().to_json();
    result[literals::Volume::CAPACITY_SOURCES] = get_capacity_sources().to_json();
    result[literals::Volume::BOOTABLE] = is_bootable();
    result[literals::Volume::ENCRYPTED] = is_encrypted();
    result[literals::Volume::ENCRYPTION_TYPES] = get_encryption_types().to_json();
    result[literals::Volume::REPLICA_INFOS] = get_replica_infos().to_json();
    result[literals::Volume::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::Volume::OPTIMUM_IO_SIZE_BYTES] = get_optimum_io_size_bytes();
    result[literals::Volume::VOLUME_TYPE] = get_volume_type();
    result[literals::Volume::ACCESS_CAPABILITIES] = get_access_capabilities().to_json();
    result[literals::Volume::OEM] = get_oem().to_json();
    return result;
}


Volume Volume::from_json(const json::Json& json) {
    Volume volume{};

    volume.set_status(attribute::Status::from_json(json[literals::Volume::STATUS]));
    volume.set_name(json[literals::Volume::NAME]);
    volume.set_description(json[literals::Volume::DESCRIPTION]);
    volume.set_collections(Collections::from_json(json[literals::Volume::COLLECTIONS]));
    volume.set_block_size_bytes(json[literals::Volume::BLOCK_SIZE_BYTES]);
    volume.set_capacity(attribute::Capacity::from_json(json[literals::Volume::CAPACITY]));
    volume.set_capacity_sources(CapacitySources::from_json(json[literals::Volume::CAPACITY_SOURCES]));
    volume.set_bootable(json[literals::Volume::BOOTABLE]);
    volume.set_encrypted(json[literals::Volume::ENCRYPTED]);
    volume.set_encryption_types(EncryptionTypes::from_json(json[literals::Volume::ENCRYPTION_TYPES]));
    volume.set_replica_infos(ReplicaInfos::from_json(json[literals::Volume::REPLICA_INFOS]));
    volume.set_identifiers(Identifiers::from_json(json[literals::Volume::IDENTIFIERS]));
    volume.set_optimum_io_size_bytes(json[literals::Volume::OPTIMUM_IO_SIZE_BYTES]);
    volume.set_volume_type(json[literals::Volume::VOLUME_TYPE]);
    volume.set_access_capabilities(AccessCapabilities::from_json(json[literals::Volume::ACCESS_CAPABILITIES]));
    volume.set_oem(attribute::Oem::from_json(json[literals::Volume::OEM]));

    return volume;
}


bool Volume::is_volume_connected_to_endpoint(const Uuid& volume_uuid) {
    auto endpoint_uuids = module::get_manager<Endpoint>().get_keys();
    return Volume::is_volume_connected_to_endpoint(endpoint_uuids, volume_uuid);
}


bool Volume::is_volume_connected_to_endpoint(const std::vector<Uuid>& endpoint_uuids, const Uuid& volume_uuid) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (Endpoint::has_entity(endpoint_uuid, volume_uuid)) {
            return true;
        }
    }
    return false;
}


bool Volume::is_volume_shared_over_fabrics(const Uuid& volume_uuid) {
    // Check if the endpoint is in a zone and if that zone has an initiator endpoint
    for (const auto& zone_uuid : module::get_manager<Zone>().get_keys()) {
        auto endpoints = module::get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid);
        if (Volume::is_volume_connected_to_endpoint(endpoints, volume_uuid)) {
            // Local lamba function to check for an initiator in the same zone
            auto is_initiator_endpoint = [](const Uuid& endpoint_uuid) {
                return Endpoint::is_initiator(endpoint_uuid);
            };
            if (find_if(endpoints.cbegin(), endpoints.cend(), is_initiator_endpoint) != endpoints.cend()) {
                return true;
            }
        }
    }

    return false;
}


bool Volume::is_volume_snapshot(const Uuid& volume_uuid) {
    return Volume::is_volume_snapshot(module::get_manager<Volume>().get_entry(volume_uuid));
}


bool Volume::is_volume_snapshot(const agent_framework::model::Volume& volume) {
    auto replica_infos = volume.get_replica_infos();
    return std::any_of(replica_infos.begin(), replica_infos.end(), [](const attribute::ReplicaInfo& info) {
        return (
            info.get_replica_type() == enums::ReplicaType::Snapshot &&
            info.get_replica_role() == enums::ReplicaRole::Target
        );
    });
}

bool Volume::is_source_for_another_volume(const Volume& volume) {
    const auto& replica_infos = volume.get_replica_infos();
    for(const auto& replica_info : replica_infos) {
        if (replica_info.get_replica_role() == enums::ReplicaRole::Source &&
            replica_info.get_replica_type() == enums::ReplicaType::Snapshot) {
            return true;
        }
    }
    return false;
}
