/*!
 * @brief Volume discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file lvm_volume_discoverer.cpp
 */

#include "discovery/discoverers/lvm_volume_discoverer.hpp"
#include "discovery/builders/lvm_volume_builder.hpp"



using namespace agent::storage::discovery;

namespace {

std::vector<agent_framework::model::Volume>::iterator
get_volume_by_uuid(std::vector<agent_framework::model::Volume>& volumes, const Uuid& uuid) {
    return std::find_if(volumes.begin(), volumes.end(), [&uuid](const agent_framework::model::Volume& volume) {
        return volume.get_uuid() == uuid;
    });
}

}


std::vector<agent_framework::model::Volume> LvmVolumeDiscoverer::discover(const Uuid& parent_uuid) {

    std::vector<agent_framework::model::Volume> volumes{};
    std::map<std::string, Uuid> name_to_uuid{};
    std::map<Uuid, std::string> uuid_to_master{};
    std::vector<::storage::lvm::model::VolumeGroup> volume_groups{};

    try {
        volume_groups = m_context->lvm_api->discover_volume_groups(::storage::lvm::DiscoveryType::VG_LV);
    }
    catch (const std::exception& ex) {
        log_error("lvm-discovery", ex.what());
        throw;
    }

    for (const auto& volume_group : volume_groups) {
        for (const auto& logical_volume : volume_group.get_logical_volumes()) {
            log_debug("lvm-discovery", logical_volume.to_string());
            auto volume = LvmVolumeBuilder::build_default(parent_uuid);
            LvmVolumeBuilder::update_with_lvm(volume, logical_volume);

            volume.set_name(logical_volume.get_name());

            /* Add entry to the map [ LV name <-> LV UUID] */
            name_to_uuid[logical_volume.get_name()] = volume.get_uuid();

            /* If LV is snapshot, store the master [LV UUID <-> LV master name] */
            if (logical_volume.is_snapshot()) {
                uuid_to_master[volume.get_uuid()] = logical_volume.get_master_name();
            }

            volumes.emplace_back(std::move(volume));
        }
    }

    /* Resolve LV master UUIDs for each snapshot logical volume */
    for (const auto& item : uuid_to_master) {
        const auto& volume_uuid = item.first;
        const auto& master_name = item.second;

        auto it = ::get_volume_by_uuid(volumes, volume_uuid);
        if (it != volumes.end()) {
            const auto& master_uuid = name_to_uuid[master_name];
            LvmVolumeBuilder::add_replica_info(*it,
                                               agent_framework::model::enums::ReplicaType::Snapshot,
                                               agent_framework::model::enums::ReplicaRole::Target,
                                               master_uuid,
                                               agent_framework::model::enums::ReplicaReadOnlyAccess::SourceElement);

            auto master_it = ::get_volume_by_uuid(volumes, master_uuid);
            if (master_it != volumes.end()) {
                LvmVolumeBuilder::add_replica_info(*master_it,
                                                   agent_framework::model::enums::ReplicaType::Snapshot,
                                                   agent_framework::model::enums::ReplicaRole::Source,
                                                   it->get_uuid(),
                                                   agent_framework::model::enums::ReplicaReadOnlyAccess::SourceElement);
            }
            else {
                log_critical("lvm-discovery", "Master volume's UUID is missing when matching with snapshot volume!");
            }
        }
        else {
            log_critical("lvm-discovery", "Volume's UUID is missing when matching with master volume!");
        }
    }

    return volumes;
}

