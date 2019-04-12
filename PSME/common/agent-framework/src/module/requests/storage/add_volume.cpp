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
 * @file requests/storage/add_volume.cpp
 * @brief Storage AddVolume request implementation
 * */

#include "agent-framework/module/requests/storage/add_volume.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;


AddVolume::AddVolume(
    const OptionalField<enums::VolumeType>& volume_type,
    const Identifiers& identifiers,
    std::int64_t capacity_bytes,
    const attribute::Array<attribute::CapacitySource>& capacity_sources,
    const attribute::Array<enums::AccessCapability>& access_capabilities,
    const OptionalField<bool>& bootable,
    const attribute::Array<attribute::ReplicaInfo>& replica_infos,
    const attribute::Oem& oem) :
    m_volume_type(volume_type),
    m_identifiers(identifiers),
    m_capacity_bytes(capacity_bytes),
    m_capacity_sources(capacity_sources),
    m_access_capabilities(access_capabilities),
    m_bootable(bootable),
    m_replica_infos(replica_infos),
    m_oem(oem) {}


json::Json AddVolume::to_json() const {
    json::Json json = json::Json();
    json[literals::Volume::VOLUME_TYPE] = get_volume_type();
    json[literals::Volume::IDENTIFIERS] = get_identifiers().to_json();
    json[literals::Volume::CAPACITY_BYTES] = get_capacity_bytes();
    json[literals::Volume::CAPACITY_SOURCES] = get_capacity_sources().to_json();
    json[literals::Volume::ACCESS_CAPABILITIES] = get_access_capabilities().to_json();
    json[literals::Volume::BOOTABLE] = get_bootable();
    json[literals::Volume::REPLICA_INFOS] = get_replica_infos().to_json();
    json[literals::Volume::OEM] = get_oem().to_json();
    return json;
}


AddVolume AddVolume::from_json(const json::Json& json) {
    return AddVolume{
        json[literals::Volume::VOLUME_TYPE],
        Identifiers::from_json(json[literals::Volume::IDENTIFIERS]),
        json[literals::Volume::CAPACITY_BYTES],
        attribute::Array<attribute::CapacitySource>::from_json(json[literals::Volume::CAPACITY_SOURCES]),
        attribute::Array<enums::AccessCapability>::from_json(json[literals::Volume::ACCESS_CAPABILITIES]),
        json[literals::Volume::BOOTABLE],
        attribute::Array<attribute::ReplicaInfo>::from_json(json[literals::Volume::REPLICA_INFOS]),
        attribute::Oem::from_json(json[literals::Volume::OEM])
    };
}
