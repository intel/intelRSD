/*!
 * @brief LVM Storage pool discoverer implementation.
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
 * @file lvm_storage_pool_discoverer.cpp
 */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/module/storage_components.hpp"

#include "discovery/discoverers/lvm_storage_pool_discoverer.hpp"
#include "discovery/builders/lvm_storage_pool_builder.hpp"

#include "sysfs/construct_dev_path.hpp"



using namespace agent_framework;
using namespace agent::storage;
using namespace agent::storage::discovery;

namespace {

bool is_matching_drive(AgentContext::SPtr context,
                       const model::Drive& drive,
                       const std::string& device_path) {

    std::string drive_path{};
    if (drive.get_name().has_value()) {
        drive_path = sysfs::construct_dev_path(drive.get_name());
    }
    else {
        THROW(agent_framework::exceptions::InvalidValue,
              "storage-agent", "Drive name has no value.");
    }

    if (drive_path == device_path) {
        return true;
    }

    auto drive_name = drive.get_name().value();
    auto handler = context->drive_handler_factory->get_handler(drive_name,
                                                               BaseDriveHandlerFactory::HandlerMode::TRY_FROM_CACHE);
    if (!handler) {
        log_error("discovery", "Could not find handler for drive: " << drive_name);
        return false;
    }

    auto drive_data = handler->get_drive_data();
    for (const auto& namespace_name : drive_data.namespaces) {
        if (sysfs::construct_dev_path(namespace_name) == device_path) {
            return true;
        }
    }
    return false;
}

}


std::vector<model::StoragePool> LvmStoragePoolDiscoverer::discover(const Uuid& parent_uuid) {

    std::vector<model::StoragePool> storage_pools{};
    std::vector<::storage::lvm::model::VolumeGroup> volume_groups{};
    try {
        volume_groups = m_context->lvm_api->discover_volume_groups(::storage::lvm::DiscoveryType::VG_PV_LV);
    }
    catch (const std::exception& ex) {
        log_error("lvm-discovery", ex.what());
        return storage_pools;
    }

    module::get_m2m_manager<model::StoragePool, model::Volume>().clear_entries();
    for (const auto& volume_group : volume_groups) {
        log_debug("lvm-discovery", volume_group.to_string());
        const auto& volume_group_path = volume_group.get_name();
        auto pool = LvmStoragePoolBuilder::build_default(parent_uuid);

        pool.set_name(volume_group.get_name());
        LvmStoragePoolBuilder::update_with_lvm(pool, volume_group);

        // Clear current capacity sources
        pool.set_capacity_sources(model::attribute::Array<model::attribute::CapacitySource>());

        for (const auto& physical_volume : volume_group.get_physical_volumes()) {
            log_debug("lvm-discovery", physical_volume.to_string());
            const auto& device_path = physical_volume.get_name();
            const auto& drive_uuids = module::get_manager<model::Drive>().get_keys(
                [&device_path, this](const model::Drive& drive) {
                    return ::is_matching_drive(m_context, drive, device_path);
                });

            if (drive_uuids.empty()) {
                log_error("lvm-discovery", "Did not find drive representing physical volume '"
                    << physical_volume.get_name() << "'.");
                continue;
            }

            LvmStoragePoolBuilder::add_drive_capacity_source(pool, physical_volume.get_capacity_b(),
                                                             physical_volume.get_capacity_b() -
                                                             physical_volume.get_free_b(), drive_uuids.front());

            log_debug("lvm-discovery", "Added new capacity source ["
                << device_path << "] for storage pool [" << volume_group_path << "].");

        }

        for (const auto& logical_volume : volume_group.get_logical_volumes()) {
            const auto& volumes = module::get_manager<model::Volume>().get_keys(
                [&logical_volume](const model::Volume& volume) {
                    return logical_volume.get_name() == volume.get_name();
                });

            if (!volumes.empty()) {
                auto volume = agent_framework::module::get_manager<model::Volume>().get_entry(volumes.front());
                module::get_m2m_manager<model::StoragePool, model::Volume>().add_entry(pool.get_uuid(),
                                                                                       volume.get_uuid());
            }
            else {
                log_warning("lvm-discovery", "There are no volumes for name " << logical_volume.get_name());
            }
        }

        storage_pools.emplace_back(std::move(pool));
    }
    return storage_pools;
}

