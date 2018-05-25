/*!
 * @brief Implementation of AddVolume command.
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
 * @file add_volume.cpp
 */

#include "lvm/lvm_api.hpp"
#include "lvm/lvm_clone_task.hpp"
#include "common/utils.hpp"
#include "tree_stability/storage_stabilizer.hpp"
#include "discovery/lvm_discoverer.hpp"
#include "discovery/builders/identifier_builder.hpp"
#include "discovery/builders/volume_builder.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/eventing/utils.hpp"


using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent::storage::utils;

/*
 * bool = existence,
 * ReplicaType = type if exists,
 * Uuid = replica UUID if exists
 * */
using ReplicaTuple = std::tuple<bool, OptionalField<enums::ReplicaType>, OptionalField<Uuid>>;

namespace {

void check_supported_access_capabilities(const AddVolume::Request& request) {
    const auto& access_capabilities = request.get_access_capabilities().get_array();
    if (access_capabilities.empty()) {
        return; /* Defaults will be used */
    }

    bool is_read_only =
        (access_capabilities == std::vector<AccessCapability>{ AccessCapability::Read });

    bool is_read_write =
        (access_capabilities == std::vector<AccessCapability>{ AccessCapability::Read, AccessCapability::Write }) ||
        (access_capabilities == std::vector<AccessCapability>{ AccessCapability::Write, AccessCapability::Read });

    if (!is_read_only && !is_read_write) {
        THROW(exceptions::UnsupportedValue, "storage-agent", "Unsupported protection state!");
    }
}

void check_pool_capacity(const StoragePool& pool, const Volume& volume) {
    auto free_bytes = pool.get_capacity().get_guaranteed_bytes().value_or(0);
    auto requested_bytes = volume.get_capacity().get_allocated_bytes();
    if (requested_bytes > free_bytes) {
        THROW(exceptions::InvalidValue, "storage-agent",
              "Size of requested volume exceeds available space of pool. Please use another storage pool.");
    }
}

bool is_snapshot(const Volume& volume) {
    auto replica_infos = volume.get_replica_infos();
    return std::any_of(replica_infos.begin(), replica_infos.end(), [](const attribute::ReplicaInfo& info) {
        return info.get_replica_type() == ReplicaType::Snapshot && info.get_replica_role() == ReplicaRole::Target;
    });
}

lvm::model::CreationData build_lvm_creation_data(const Volume& volume,
                                                 const StoragePool& pool,
                                                 const Volume& replica) {
    std::string pool_name{};
    std::string replica_name{};

    try {
        /* Replica is optional so try to get path */
        replica_name = get_name_from_device_path(attribute::Identifier::get_system_path(replica));
    }
    catch (const std::logic_error&) {
        /* Replica not provided, leave empty string */
    }
    pool_name = get_name_from_device_path(attribute::Identifier::get_system_path(pool));

    lvm::model::CreationData lvm_create_data{};
    lvm_create_data.set_create_name(volume.get_uuid());
    lvm_create_data.set_volume_group(pool_name);
    lvm_create_data.set_logical_volume(replica_name);
    lvm_create_data.set_size(volume.get_capacity().get_allocated_bytes());

    bool is_master_bootable = replica.is_bootable().value_or(false);
    bool is_new_volume_bootable = volume.is_bootable().value_or(is_master_bootable);
    lvm_create_data.set_bootable(is_new_volume_bootable);

    return lvm_create_data;
}

ReplicaTuple get_replica_tuple(const attribute::Array<attribute::ReplicaInfo>& replica_infos) {
    if (replica_infos.empty()) {
        log_info("storage-agent", "Replica not provided - request to create empty volume.");
        return std::make_tuple(false, OptionalField<enums::ReplicaType>{}, Uuid{});
    }
    else {
        /* Only one possible replica, but type should be snapshot or clone */
        const auto& replica_info = replica_infos.front();
        if (replica_info.get_replica_type() == ReplicaType::Snapshot ||
            replica_info.get_replica_type() == ReplicaType::Clone) {
            return std::make_tuple(true, replica_info.get_replica_type(), replica_info.get_replica());
        }
        else if (!replica_info.get_replica_type().has_value()) {
            THROW(exceptions::InvalidValue, "storage-agent", "Replica type was not provided!");
        }
        else {
            THROW(exceptions::InvalidValue, "storage-agent", "Unsupported replica type!");
        }
    }
}

void rediscover_storage_pool(const Uuid& storage_pool_uuid) {
    auto discoverer = LvmDiscoverer();
    auto pool_ref = get_manager<StoragePool>().get_entry_reference(storage_pool_uuid);
    discoverer.rediscover_pool_capacity(pool_ref.get_raw_ref());
    auto result = get_manager<StoragePool>().add_or_update_entry(pool_ref.get_raw_ref());
    if (result == agent_framework::module::TableInterface::UpdateStatus::Updated) {
        eventing::send_update(pool_ref.get_raw_ref());
    }
}

StoragePool find_providing_pool(const attribute::Array<attribute::CapacitySource>& sources,
                                const ReplicaTuple& replica_tuple) {

    if (get_manager<StoragePool>().get_keys().empty()) {
        THROW(exceptions::InvalidValue, "storage-agent", "There is no source pool to create new volume from.");
    }

    if (!sources.empty()) {
        auto capacity_source = sources.front();
        if (!capacity_source.get_providing_pools().empty()) {
            return get_manager<StoragePool>().get_entry(capacity_source.get_providing_pools().front());
        }
    }

    /* User did not provide source pool, so find matching one */
    if (std::get<0>(replica_tuple) == false) {
        /* Use default (first) pool */
        return get_manager<StoragePool>().get_entries().front();
    }
    else {
        auto replica_type = std::get<1>(replica_tuple);
        auto replica_uuid = std::get<2>(replica_tuple);

        if (replica_type.has_value()) {
            /* Use the same pool as master pool of replica */
            auto master_pools = get_m2m_manager<StoragePool, Volume>().get_parents(replica_uuid);
            if (master_pools.empty()) {
                THROW(exceptions::InvalidValue, "storage-agent", "Missing master pool for requested replica.");
            }
            return get_manager<StoragePool>().get_entry(master_pools.front());
        }
        else {
            throw std::logic_error("Replica data should be checked before calling this method");
        }
    }
}

void create_new_volume(Volume& volume,
                       const StoragePool& pool,
                       AddVolume::Response& response,
                       bool clone,
                       const Volume& replica = Volume{}) {

    if (clone) {
        // A clone volume must have capacity greater than or equal to that of its master volume
        if (!replica.get_capacity().get_allocated_bytes().has_value()) {
            THROW(exceptions::LvmError, "storage-agent", "Master volume's capacity is unknown.");
        }

        if (volume.get_capacity().get_allocated_bytes() < replica.get_capacity().get_allocated_bytes()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "The specified capacity is too small to clone a master volume.");
        }

        VolumeBuilder::add_replica_info(volume, replica.get_uuid(),
                                        enums::ReplicaType::Clone,
                                        enums::ReplicaRole::Target);
    }

    std::shared_ptr<Volume> volume_sptr = std::make_shared<Volume>(volume);

    auto lvm_create_data = build_lvm_creation_data(volume, pool, replica);
    auto lvm_create_volume_subtask = [lvm_create_data, volume_sptr]() {
        lvm::LvmApi lvm_api{};
        auto created_volume = lvm_api.create_volume(lvm_create_data);
        VolumeBuilder::update_with_lvm(*volume_sptr, created_volume);
    };

    auto replica_uuid = replica.get_uuid();
    auto promised_response_builder = [volume_sptr, pool, clone, replica_uuid]() {
        StorageStabilizer().stabilize(*volume_sptr);
        VolumeBuilder::update_identifier_with_uuid(*volume_sptr);
        VolumeBuilder::add_capacity_source(*volume_sptr, pool.get_uuid());

        log_debug("storage-agent", "Volume added: " << volume_sptr->get_uuid());
        get_manager<Volume>().add_entry(*volume_sptr);
        get_m2m_manager<StoragePool, Volume>().add_entry(pool.get_uuid(), volume_sptr->get_uuid());
        eventing::send_event(volume_sptr->get_uuid(), Component::Volume,
                             Notification::Add, volume_sptr->get_parent_uuid());

        if (clone) {
            auto master_volume = get_manager<Volume>().get_entry_reference(replica_uuid);
            VolumeBuilder::add_replica_info(master_volume.get_raw_ref(), volume_sptr->get_uuid(),
                                            enums::ReplicaType::Clone, enums::ReplicaRole::Source);
        }

        /* Re-discover pool as its capacity changed */
        rediscover_storage_pool(pool.get_uuid());

        AddVolume::Response promised_response{};
        promised_response.set_volume(volume_sptr->get_uuid());
        return promised_response.to_json();
    };

    auto promised_exception_builder = [](const exceptions::GamiException& exception) {
        return exception;
    };

    action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_subtask(lvm_create_volume_subtask);
    if (clone) {
        task_creator.add_subtask(lvm::LvmCloneTask{lvm_create_data});
    }
    task_creator.set_promised_response(promised_response_builder);
    task_creator.set_promised_error_thrower(promised_exception_builder);

    task_creator.register_task();
    auto creation_task = task_creator.get_task();
    action::TaskRunner::get_instance().run(creation_task);

    log_info("storage-agent", "New logical volume creation started.");
    response.set_task(task_creator.get_task_resource().get_uuid());
}

void create_snapshot(Volume& volume,
                     const StoragePool& pool,
                     const Volume& replica,
                     AddVolume::Response& response) {

    if (is_snapshot(replica)) {
        THROW(exceptions::UnsupportedValue, "storage-agent", "Cannot create snapshot of a snapshot master volume.");
    }

    auto master_pools = get_m2m_manager<StoragePool, Volume>().get_parents(replica.get_uuid());
    if(std::find(master_pools.begin(), master_pools.end(), pool.get_uuid()) == master_pools.end()) {
        THROW(exceptions::InvalidValue, "storage-agent",
              "Cannot create snapshot from provided storage pool. Only master pool is supported.");
    }

    lvm::LvmApi lvm_api{};
    auto created_volume = lvm_api.create_snapshot(build_lvm_creation_data(volume, pool, replica));
    VolumeBuilder::update_with_lvm(volume, created_volume);
    VolumeBuilder::add_capacity_source(volume, pool.get_uuid());

    VolumeBuilder::add_replica_info(volume, replica.get_uuid(),
                                    enums::ReplicaType::Snapshot,
                                    enums::ReplicaRole::Target);

    StorageStabilizer().stabilize(volume);
    VolumeBuilder::update_identifier_with_uuid(volume);

    {
        auto master_volume = get_manager<Volume>().get_entry_reference(replica.get_uuid());
        VolumeBuilder::add_replica_info(master_volume.get_raw_ref(), volume.get_uuid(),
                                        enums::ReplicaType::Snapshot, enums::ReplicaRole::Source);
    }

    /* Re-discover pool as its capacity changed */
    rediscover_storage_pool(pool.get_uuid());

    log_debug("storage-agent", "Volume added: " << volume.get_uuid());
    get_manager<Volume>().add_entry(volume);
    get_m2m_manager<StoragePool, Volume>().add_entry(pool.get_uuid(), volume.get_uuid());
    response.set_volume(volume.get_uuid());
}

void add_volume(const AddVolume::Request& request, AddVolume::Response& response) {
    log_debug("storage-agent", "Adding volume...");

    /* Only default empty array, Read or Read/Write are allowed */
    check_supported_access_capabilities(request);
    auto replica_tuple = get_replica_tuple(request.get_replica_infos());

    auto parent_service = get_manager<StorageService>().get_entries().front();
    auto providing_pool = find_providing_pool(request.get_capacity_sources(), replica_tuple);
    auto volume = VolumeBuilder::build_default(parent_service.get_uuid());
    VolumeBuilder::add_identifier_with_uuid(volume);
    VolumeBuilder::update_with_request(volume, request);
    auto pool_device_path = attribute::Identifier::get_system_path(providing_pool);
    IdentifierBuilder::add_system_path_identifier(volume, pool_device_path + "/" + volume.get_uuid());

    // TODO: Implement LVM function for protection setting
    // volume.set_access_capabilities(request.get_access_capabilities());

    check_pool_capacity(providing_pool, volume);

    if (volume.get_capacity().get_allocated_bytes() <= 0) {
        THROW(exceptions::InvalidValue, "storage-agent", "Capacity of new volume cannot be negative or zero.");
    }

    if (std::get<0>(replica_tuple) == false) {
        /* Creating empty volume */
        create_new_volume(volume, providing_pool, response, false);
    }
    else {
        auto replica_type = std::get<1>(replica_tuple);
        auto replica = get_manager<Volume>().get_entry(std::get<2>(replica_tuple));
        log_debug("storage-agent", "Replica Volume UUID: " << replica.get_uuid());

        if (replica_type == ReplicaType::Snapshot) {
            create_snapshot(volume, providing_pool, replica, response);
        }
        else if (replica_type == ReplicaType::Clone) {
            create_new_volume(volume, providing_pool, response, true, replica);
        }
        else {
            throw std::logic_error("Replica data should be checked before calling this method");
        }
    }
}

}

REGISTER_COMMAND(AddVolume, ::add_volume);
