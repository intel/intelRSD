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
 * @file spdk_volume_discoverer.cpp
 */

#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/module/storage_components.hpp"

#include "spdk/spdk_api.hpp"
#include "spdk/model/bdev.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"

#include "discovery/discoverers/spdk_volume_discoverer.hpp"
#include "discovery/builders/spdk_volume_builder.hpp"
#include "spdk_stabilizer.hpp"
#include "database_keys.hpp"



using namespace agent::spdk::discovery;
using namespace agent_framework::model;
using namespace spdk;
using namespace spdk::model;

namespace {

agent_framework::model::Volume& get_volume_by_uuid(std::vector<agent_framework::model::Volume>& volumes,
                                                   const Uuid& uuid) {
    auto it = std::find_if(volumes.begin(), volumes.end(), [&uuid](const agent_framework::model::Volume& volume) {
        return volume.get_uuid() == uuid;
    });
    if (it == volumes.end()) {
        throw std::runtime_error("Could not find volume with UUID '" + uuid + "' in discovered volumes.");
    }
    return *it;
}

}


std::vector<Volume> SpdkVolumeDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<agent_framework::model::Volume> volumes{};
    std::vector<model::Bdev> bdevs{};
    std::map<std::string, Uuid> identifier_to_uuid_map{};
    std::vector<std::pair<std::string, std::string>> source_and_clone_pairs{};

    try {
        bdevs = m_context->spdk_api->get_bdevs();
    }
    catch (const std::exception& exception) {
        log_error("spdk-discovery", "Failed to call get_bdevs method on SPDK: " << exception.what());
    }

    for (const auto& bdev : bdevs) {
        try {
            if (bdev.get_product_name() != BdevProductName::LOGICAL_VOLUME) {
                continue;
            }

            log_debug("spdk-discovery", "\n" << bdev.to_string());
            auto volume = SpdkVolumeBuilder::build_default(parent_uuid);
            SpdkVolumeBuilder::update_with_spdk(volume, bdev);

            auto volume_identifier = attribute::Identifier::get_uuid(volume);
            auto stabilized_volume_uuid = SpdkStabilizer().dry_stabilize(volume);

            // FIXME: Workaround for SPDK limitation with deletion of snapshots in chain
            std::string is_hidden{};
            std::string spdk_uuid{};

            try {
                auto db = agent_framework::database::VolumeEntity(stabilized_volume_uuid);
                is_hidden = db.get(VOLUME_HIDDEN_PROPERTY);
                spdk_uuid = db.get(VOLUME_SPDK_UUID_PROPERTY);
            }
            catch (const std::exception& exception) {
                log_debug("spdk-discovery", "Volume database error: " << exception.what());
            }

            if (is_hidden == std::to_string(1) && spdk_uuid == volume.get_name()) {
                m_context->hidden_volumes.insert(stabilized_volume_uuid);
            }
            else {
                try {
                    // Visible volumes do not need database
                    agent_framework::database::VolumeEntity::remove(stabilized_volume_uuid);
                }
                catch (const std::exception& exception) {
                    log_debug("spdk-discovery", "Volume database error: " << exception.what());
                }
            }

            if (m_context->hidden_volumes.count(stabilized_volume_uuid)) {
                log_debug("spdk-discovery", "BDEV " << volume_identifier << " is marked as hidden.");
                continue;
            }
            // FIXME: End of workaround snippet

            identifier_to_uuid_map.insert({volume_identifier, volume.get_uuid()});

            auto lvol_driver = bdev.get_driver_specific<drivers::LvolDriver>();
            if (!lvol_driver) {
                log_warning("spdk-discovery", "LVOL " << bdev.get_name() << " driver specific data not available!");
            }
            else {
                auto lvol_store_uuid = lvol_driver->get_lvol_store_uuid();
                auto storage_pools = agent_framework::module::get_manager<StoragePool>()
                    .get_keys([lvol_store_uuid](const StoragePool& storage_pool) {
                        return attribute::Identifier::get_uuid(storage_pool) == lvol_store_uuid;
                    });
                for (const auto& storage_pool_uuid : storage_pools) {
                    SpdkVolumeBuilder::set_storage_pool_capacity_source(volume, storage_pool_uuid);
                    agent_framework::module::get_m2m_manager<StoragePool, Volume>()
                        .add_entry(storage_pool_uuid, volume.get_uuid());
                    log_debug("spdk-discovery",
                              "Added Volume " << volume.get_uuid() << " as child of Storage Pool " << storage_pool_uuid);
                }

                if (lvol_driver->is_snapshot()) {
                    SpdkVolumeBuilder::add_replica_info(volume,
                                                        enums::ReplicaType::Snapshot,
                                                        enums::ReplicaRole::Target,
                                                        OptionalField<Uuid>{},
                                                        enums::ReplicaReadOnlyAccess::ReplicaElement);

                    for (const auto& clone_name : lvol_driver->get_clones().value_or_default()) {
                        source_and_clone_pairs.push_back({volume_identifier, clone_name});
                    }
                }
            }

            volumes.emplace_back(std::move(volume));
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery", "Failed to convert volume model from SPDK BDEV: " << exception.what());
        }
    }

    for (const auto& source_and_clone_name : source_and_clone_pairs) {
        try {
            Uuid source_uuid = identifier_to_uuid_map.at(source_and_clone_name.first);
            Uuid replica_uuid = identifier_to_uuid_map.at(source_and_clone_name.second);

            auto& source_volume = ::get_volume_by_uuid(volumes, source_uuid);
            SpdkVolumeBuilder::add_replica_info(source_volume,
                                                enums::ReplicaType::Clone,
                                                enums::ReplicaRole::Source,
                                                replica_uuid,
                                                enums::ReplicaReadOnlyAccess::SourceElement);

            auto& cloned_volume = ::get_volume_by_uuid(volumes, replica_uuid);
            SpdkVolumeBuilder::add_replica_info(cloned_volume,
                                                enums::ReplicaType::Clone,
                                                enums::ReplicaRole::Target,
                                                source_uuid,
                                                enums::ReplicaReadOnlyAccess::SourceElement);

            log_debug("spdk-discovery", "Volume " << source_and_clone_name.second
                                                  << " is clone of " << source_and_clone_name.first);
        }
        catch (const std::exception& exception) {
            log_error("spdk-agent", "Cannot set replica information for volume "
                << source_and_clone_name.first << " and replica "
                << source_and_clone_name.second << ": " << exception.what());
        }
    }

    return volumes;
}
