/*!
 * @brief Implementation of AddVolume command.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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


#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/eventing/utils.hpp"

#include "spdk_stabilizer.hpp"
#include "spdk_agent_commands.hpp"
#include "spdk_converter.hpp"

#include "discovery/builders/spdk_volume_builder.hpp"
#include "discovery/builders/spdk_storage_pool_builder.hpp"
#include "command/helpers/helpers.hpp"



using namespace agent::spdk;
using namespace agent::spdk::discovery;
using namespace agent::spdk::utils;
using namespace agent_framework;
using namespace agent_framework::model;

namespace {

/*
 * bool = existence,
 * ReplicaType = type if exists,
 * Uuid = replica UUID if exists
 * */


using ReplicaTuple = std::tuple<bool, OptionalField<enums::ReplicaType>, OptionalField<Uuid>>;


void throw_if_readonly(const AddVolume::Request& request) {
    const auto& access_capabilities = request.get_access_capabilities().get_array();
    if (access_capabilities.empty()) {
        return; /* Defaults will be used */
    }

    bool is_read_write =
        (access_capabilities ==
         std::vector<enums::AccessCapability>{enums::AccessCapability::Read, enums::AccessCapability::Write}) ||
        (access_capabilities ==
         std::vector<enums::AccessCapability>{enums::AccessCapability::Write, enums::AccessCapability::Read});

    if (!is_read_write) {
        THROW(exceptions::UnsupportedValue, "spdk-agent", "Unsupported access capabilities.");
    }
}


void throw_if_readwrite(const AddVolume::Request& request) {
    const auto& access_capabilities = request.get_access_capabilities().get_array();
    if (access_capabilities.empty()) {
        return; /* Defaults will be used */
    }

    if (access_capabilities != std::vector<enums::AccessCapability>{enums::AccessCapability::Read}) {
        THROW(exceptions::UnsupportedValue, "spdk-agent", "Unsupported access capabilities.");
    }
}


ReplicaTuple get_replica_tuple(const attribute::Array<attribute::ReplicaInfo>& replica_infos) {
    if (replica_infos.empty()) {
        log_info("spdk-agent", "Replica not provided - request to create empty volume.");
        return std::make_tuple(false, OptionalField<enums::ReplicaType>{}, OptionalField<Uuid>{});
    }
    else {
        /* Only one possible replica, but type should be snapshot or clone */
        const auto& replica_info = replica_infos.front();

        if (replica_info.get_replica_type() == enums::ReplicaType::Snapshot ||
            replica_info.get_replica_type() == enums::ReplicaType::Clone) {
            return std::make_tuple(true, replica_info.get_replica_type(), replica_info.get_replica());
        }
        else if (!replica_info.get_replica_type().has_value()) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Replica type was not provided!");
        }
        else {
            THROW(exceptions::InvalidValue, "spdk-agent", "Unsupported replica type!");
        }
    }
}


StoragePool find_providing_pool(const attribute::Array<attribute::CapacitySource>& sources,
                                const ReplicaTuple& replica_tuple) {

    if (module::get_manager<StoragePool>().get_keys().empty()) {
        THROW(exceptions::InvalidValue, "spdk-agent", "There is no source pool to create new volume from.");
    }

    OptionalField<Uuid> requested_storage_pool{};
    if (!sources.empty()) {
        auto capacity_source = sources.front();
        if (!capacity_source.get_providing_pools().empty()) {
            requested_storage_pool = capacity_source.get_providing_pools().front();
        }
    }

    if (std::get<0>(replica_tuple) == false) {
        if (requested_storage_pool.has_value()) {
            return module::get_manager<StoragePool>().get_entry(requested_storage_pool);
        }

        /* User did not provide source pool, so use default (first) pool */
        return module::get_manager<StoragePool>().get_entries().front();
    }
    else {
        auto replica_type = std::get<1>(replica_tuple);
        auto replica_uuid = std::get<2>(replica_tuple);

        if (replica_type.has_value()) {
            /* Use the same pool as master pool of replica */
            auto master_pools = module::get_m2m_manager<StoragePool, Volume>().get_parents(replica_uuid);
            if (master_pools.empty()) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Missing master pool for requested replica.");
            }

            auto master_pool = module::get_manager<StoragePool>().get_entry(master_pools.front());
            if (requested_storage_pool.has_value() && master_pool.get_uuid() != requested_storage_pool) {
                THROW(exceptions::InvalidValue, "spdk-agent",
                      "Requested storage pool has to be the same as source volume's storage pool.");
            }

            return master_pool;
        }
        else {
            throw std::logic_error("Replica data should be checked before calling this method.");
        }
    }
}


std::string create_new_volume(AgentContext::SPtr context,
                              const std::string& volume,
                              const std::int64_t& capacity,
                              const std::string& lvol_store_uuid) {
    try {
        return context->spdk_api->construct_lvol_bdev_by_store_uuid(volume, capacity, lvol_store_uuid, false,
                                                                    ::spdk::model::ClearMethod::WRITE_ZEROES);
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent", "Could not create new volume: " + std::string{exception.what()});
    }
}


std::string create_snapshot_volume(AgentContext::SPtr context,
                                   const std::string& source,
                                   const std::string& snapshot) {
    try {
        return context->spdk_api->snapshot_lvol_bdev(source, snapshot);
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent",
              "Could not create snapshot volume: " + std::string{exception.what()});
    }
}


std::string create_clone_volume(AgentContext::SPtr context,
                                const std::string& source_snapshot,
                                const std::string& clone) {
    try {
        return context->spdk_api->clone_lvol_bdev(source_snapshot, clone);
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent", "Could not create clone volume: " + std::string{exception.what()});
    }
}


void throw_if_uuid_is_not_unique(const std::string& uuid) {
    for (const auto& volume : module::get_manager<Volume>().get_entries()) {
        if (attribute::Identifier::get_uuid(volume) == uuid) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Volume with specified UUID already exists.");
        }
    }
}


void add_volume(AddVolume::ContextPtr context, const AddVolume::Request& request, AddVolume::Response& response) {
    log_info("spdk-agent", "Adding volume...");

    auto replica_tuple = get_replica_tuple(request.get_replica_infos());

    auto providing_pool = find_providing_pool(request.get_capacity_sources(), replica_tuple);
    auto parent_service = module::get_manager<StorageService>().get_entries().front();

    auto volume = agent::spdk::discovery::SpdkVolumeBuilder::build_default(parent_service.get_uuid());
    volume.set_oem(request.get_oem());

    if (request.get_bootable().has_value()) {
        THROW(exceptions::UnsupportedField, "spdk-agent", "Bootable flag is not supported by SPDK.",
              literals::Volume::BOOTABLE, request.get_bootable());
    }

    if (request.get_capacity_bytes() <= 0) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Capacity of new volume cannot be negative or zero. Requested " +
              std::to_string(request.get_capacity_bytes()) + ".");
    }

    if (!StoragePool::can_allocate_bytes(providing_pool, request.get_capacity_bytes())) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Size of requested volume exceeds available space of pool. Please use another storage pool.");
    }

    auto lvol_store_uuid = attribute::Identifier::get_uuid(providing_pool);
    std::string lvol_bdev_uuid{};
    std::lock_guard<std::mutex> lock{context->spdk_mutex};

    OptionalField<std::string> lvol_bdev_name{};
    if (!request.get_identifiers().empty()) {
        try {
            lvol_bdev_name = attribute::Identifier::get_uuid(request);
        }
        catch (const std::exception&) {
            THROW(exceptions::SpdkError, "spdk-agent", "Only UUID identifier is supported.");
        }
    }

    if (lvol_bdev_name.has_value()) {
        throw_if_uuid_is_not_unique(lvol_bdev_name);
    }
    else {
        lvol_bdev_name = volume.get_uuid();
    }

    if (std::get<0>(replica_tuple) == false) {
        throw_if_readonly(request);

        /* Creating empty volume */
        lvol_bdev_uuid = create_new_volume(context, lvol_bdev_name, request.get_capacity_bytes(), lvol_store_uuid);
    }
    else {
        auto replica_type = std::get<1>(replica_tuple);
        auto replica = module::get_manager<Volume>().get_entry(std::get<2>(replica_tuple));
        auto source_bdev_name = model::attribute::Identifier::get_uuid(replica);
        log_debug("spdk-agent", "Replica Volume UUID: " << replica.get_uuid());

        if (request.get_capacity_bytes() != replica.get_capacity().get_allocated_bytes()) {
            THROW(exceptions::InvalidValue, "spdk-agent",
                  "Requested capacity does not match source volume's capacity!");
        }

        auto source_bdev_alias = SpdkConverter::make_alias(providing_pool.get_name(), source_bdev_name);
        if (replica_type == enums::ReplicaType::Snapshot) {
            throw_if_readwrite(request);
            if (Volume::is_volume_snapshot(replica)) {
                THROW(exceptions::InvalidValue, "spdk-agent",
                      "Cannot create snapshot volume from snapshot volume!");
            }

            lvol_bdev_uuid = create_snapshot_volume(context, source_bdev_alias, lvol_bdev_name);
        }
        else if (replica_type == enums::ReplicaType::Clone) {
            throw_if_readonly(request);
            if (!Volume::is_volume_snapshot(replica)) {
                THROW(exceptions::InvalidValue, "spdk-agent",
                      "Cannot create clone from volume type other than snapshot!");
            }
            lvol_bdev_uuid = create_clone_volume(context, source_bdev_alias, lvol_bdev_name);
        }
    }

    // Refresh volume info but without replica relations yet
    try {
        auto lvol_bdev = context->spdk_api->get_bdevs(lvol_bdev_uuid).at(0);
        agent::spdk::discovery::SpdkVolumeBuilder::update_with_spdk(volume, lvol_bdev);

        SpdkStabilizer{}.stabilize(volume);
        response.set_volume(volume.get_uuid());

        agent::spdk::discovery::SpdkVolumeBuilder::set_storage_pool_capacity_source(volume,
                                                                                    providing_pool.get_uuid());

        module::get_manager<Volume>().add_entry(volume);
        module::get_m2m_manager<StoragePool, Volume>().add_entry(providing_pool.get_uuid(), volume.get_uuid());
        log_info("spdk-agent", "Volume added: " << volume.get_uuid());
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent",
              "Could not refresh LVOL BDEV data: " + std::string{exception.what()});
    }

    agent::spdk::command::refresh_storage_pool(context, providing_pool.get_uuid());
    agent::spdk::command::refresh_volumes(context, parent_service.get_uuid());
}

}

REGISTER_SPDK_COMMAND(AddVolume, ::add_volume);
