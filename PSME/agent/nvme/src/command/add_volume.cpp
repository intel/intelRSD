/*!
 * @brief Implementation of AddVolume command.
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
 * @file add_volume.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/storage_components.hpp"

#include "loader/config.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "agent-framework/eventing/events_queue.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::tools;
using namespace agent::nvme::loader;
using namespace agent::nvme;

namespace {

Uuid validate_request(const AddVolume::Request& req) {
    if (req.get_volume_type() != enums::VolumeType::RawDevice) {
        THROW(exceptions::InvalidValue, "nvme-agent", "RawDevice volume type is the only supported type");
    }
    if (!req.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Cannot create Volumes with specified identifiers.");
    }
    if (req.get_capacity_bytes() <= 0) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Invalid number of capacity bytes requested");
    }
    const auto& sources = req.get_capacity_sources();
    if (req.get_access_capabilities().size() > 0) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Access capabilities array is not supported");
    }
    if (req.get_bootable().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Bootable flag is not supported");
    }
    if (!req.get_replica_infos().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Replica infos is not supported");
    }
    if (sources.size() > 1) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Capacity sources array must have at most 1 element");
    }
    if (!sources.empty()) {
        const auto& cs = sources.front();
        if (!cs.get_providing_drives().empty()) {
            THROW(exceptions::InvalidValue, "nvme-agent", "Providing drives is not supported");
        }
        if (!cs.get_providing_volumes().empty()) {
            THROW(exceptions::InvalidValue, "nvme-agent", "Providing volumes is not supported");
        }
        const auto& pools = cs.get_providing_pools();
        if (pools.size() > 1) {
            THROW(exceptions::InvalidValue, "nvme-agent", "Providing pools array must have at most one element");
        }
        Uuid pool_uuid{};
        if (!pools.empty()) {
            pool_uuid = pools.front();
            // check if pool actually exists, this will throw InvalidUUID if not found
            get_manager<StoragePool>().get_entry(pool_uuid);
        }
        return pool_uuid;
    }
    return {};
}


std::pair<bool, std::string> create_volume(AddVolume::ContextPtr context,
                                           const std::string& drive_name,
                                           const std::uint64_t bytes,
                                           std::uint64_t& block_size_bytes,
                                           std::uint64_t& volume_size_bytes) {
    try {
        auto handler = context->drive_handler_factory->get_handler(drive_name);
        if (!handler) {
            THROW(exceptions::NvmeError, "nvme-agent", "Unable to get handler for drive " + drive_name);
        }
        handler->load();
        handler->initialize_drive();
        auto volume_name = handler->create_volume(bytes);
        auto vd = handler->get_volume_data(volume_name);

        if (vd.block_size_bytes.has_value()) {
            block_size_bytes = vd.block_size_bytes.value();
            if (vd.size_lba.has_value()) {
                volume_size_bytes = vd.size_lba.value() * vd.block_size_bytes.value();
            }
        }

        return {true, volume_name};
    }
    catch (const std::exception& e) {
        log_debug("nvme-agent", "Failed creating partition on drive " + drive_name + " with error: " + e.what());
    }

    return {false, ""};
}


std::string create_volume_in_pool(AddVolume::ContextPtr context, const AddVolume::Request& req,
                                  std::uint64_t& block_size_bytes, std::uint64_t& volume_size_bytes,
                                  Uuid& pool_uuid) {

    std::vector<Uuid> considered_pools{};
    if (!pool_uuid.empty()) {
        // storage pool was provided - use it
        considered_pools.push_back(pool_uuid);
    }
    else {
        // storage pool not provided - consider all available
        considered_pools = get_manager<StoragePool>().get_keys();
    }

    for (const auto& considered_pool_uuid : considered_pools) {
        auto pool = get_manager<StoragePool>().get_entry(considered_pool_uuid);
        if (!pool.get_name().has_value()) {
            log_warning("nvme-agent", "Storage Pool '" << pool.get_uuid() << "' has no name.");
            continue;
        }
        // Try to create volume on each pool, if the operation fail use next one
        auto status = create_volume(context, pool.get_name(),
                                    req.get_capacity_bytes(), block_size_bytes, volume_size_bytes);

        if (status.first) {
            // Volume successfully created. Return volume name and pool UUID.
            pool_uuid = considered_pool_uuid;
            return status.second;
        }
    }

    if (pool_uuid.empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent",
            "Could not create partition on any available drive. Try to provide a valid storage pool or change the requested capacity.");
    }
    else {
        THROW(exceptions::InvalidValue, "nvme-agent",
            "Could not create partition on the requested drive. Try to provide another storage pool or change the requested capacity.");
    }
}


void add_volume(AddVolume::ContextPtr context, const AddVolume::Request& req, AddVolume::Response& rsp) {
    log_info("nvme-agent", "Creating volume....");
    auto pool_uuid = validate_request(req);
    auto storage_services = get_manager<StorageService>().get_keys();
    if (storage_services.size() != 1) {
        THROW(exceptions::NvmeError, "nvme-agent", "Storage Service was not yet created.");
    }

    Volume volume{storage_services.front()};
    std::uint64_t block_size_bytes{};
    std::uint64_t volume_size_bytes{};
    std::string volume_name = create_volume_in_pool(context, req, block_size_bytes, volume_size_bytes, pool_uuid);

    volume.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    volume.set_volume_type(req.get_volume_type());
    volume.set_access_capabilities(req.get_access_capabilities());
    volume.set_name(volume_name);

    // update volume providing pools with the appropriate pool_uuid
    attribute::Array<Uuid> providing_pools{pool_uuid};
    attribute::Array<attribute::CapacitySource> capacity_sources = {attribute::CapacitySource{}};
    capacity_sources[0].set_providing_pools(providing_pools);
    volume.set_capacity_sources(capacity_sources);

    if (block_size_bytes > 0) {
        volume.set_block_size_bytes(block_size_bytes);
    }
    attribute::Capacity capability{};
    if (volume_size_bytes > 0) {
        capability.set_allocated_bytes(volume_size_bytes);
    }
    volume.set_capacity(capability);

    NvmeStabilizer().stabilize(volume);
    // add stabilized UUID identifier to the resource
    volume.add_identifier({volume.get_uuid(), enums::IdentifierType::UUID});

    get_m2m_manager<StoragePool, Volume>().add_entry(pool_uuid, volume.get_uuid());
    get_manager<Volume>().add_entry(volume);

    rsp.set_volume(volume.get_uuid());
    log_info("nvme-agent", "Added volume with UUID '" + volume.get_uuid() + "'");

    update_storage_pool_consumed_capacity(pool_uuid);
    attribute::EventData edat{};
    edat.set_component(pool_uuid);
    edat.set_parent(storage_services.front());
    edat.set_type(StoragePool::get_component());
    edat.set_notification(enums::Notification::Update);
    eventing::EventsQueue::get_instance()->push_back(edat);
}

}

REGISTER_NVME_COMMAND(AddVolume, ::add_volume);
