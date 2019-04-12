/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_volume_attributes.cpp
 * @brief Set Volume Attributes command
 */

#include "agent-framework/module/requests/validation/storage.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/lvm_storage_pool_discoverer.hpp"
#include "discovery/discoverers/lvm_volume_discoverer.hpp"
#include "discovery/builders/lvm_volume_builder.hpp"
#include "agent/command/set_volume_attributes.hpp"
#include "agent/utils/utils.hpp"

#include "lvm/lvm_erase_task.hpp"
#include "lvm/lvm_attribute.hpp"

#include "sysfs/construct_dev_path.hpp"


using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent::storage::utils;

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;
using namespace agent_framework::model::requests::validation;

namespace {

void process_bootable_tag(AgentContext::SPtr context, const Uuid& volume_uuid, bool value) {
    auto volume = get_manager<Volume>().get_entry_reference(volume_uuid);
    auto storage_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume->get_uuid());
    if (storage_pools.empty()) {
        THROW(InvalidValue, "storage-agent", "Storage pool for volume not found!");
    }
    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pools.front());

    try {

        auto vg_name = storage_pool.get_name();
        auto lv_name = volume->get_name();

        if (value) {
            context->lvm_api->add_logical_volume_tag(vg_name, lv_name, ::storage::lvm::attribute::LV_BOOTABLE_ATTR);
        }
        else {
            context->lvm_api->remove_logical_volume_tag(vg_name, lv_name, ::storage::lvm::attribute::LV_BOOTABLE_ATTR);
        }
    }
    catch (const std::exception& ex) {
        THROW(LvmError, "lvm", ex.what());
    }

    volume->set_bootable(value);
    log_debug("storage-agent", "Set 'bootable' attribute to " << std::boolalpha << value << ".");
}


void process_capacity_bytes(AgentContext::SPtr context,
                            const Uuid& volume_uuid,
                            std::int64_t requested_capacity) {

    auto volume = get_manager<Volume>().get_entry(volume_uuid);
    auto pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume_uuid);
    if (pools.empty()) {
        THROW(LvmError, "storage-agent", "Volume is not assigned to storage pool!");
    }
    auto pool = get_manager<StoragePool>().get_entry(pools.front());

    std::int64_t actual_capacity = volume.get_capacity().get_allocated_bytes();
    log_info("storage-agent", "Request to resize volume from " << actual_capacity << " to " << requested_capacity);

    if (requested_capacity < actual_capacity) {
        THROW(UnsupportedValue, "storage-agent", "Only volume extension is supported.");
    }
    if (!StoragePool::can_allocate_bytes(pool, requested_capacity)) {
        THROW(InvalidValue, "storage-agent",
              "Requested capacity to resize exceeds available capacity of pool.");
    }

    if (Volume::is_volume_shared_over_fabrics(volume_uuid)) {
        THROW(InvalidValue, "storage-agent",
              "Cannot resize volume which is shared over fabrics. Remove related endpoints from zone first.");
    }

    auto volume_group_name = pool.get_name();
    auto logical_volume_name = volume.get_name();

    try {
        auto lv_resized = context->lvm_api->resize_volume(volume_group_name, logical_volume_name, requested_capacity);
        log_debug("storage-agent", "Volume successfully resized: \n" + lv_resized.to_string());

        {
            auto volume_ref = get_manager<Volume>().get_entry_reference(volume_uuid);
            discovery::LvmVolumeBuilder::update_with_lvm(volume_ref.get_raw_ref(), lv_resized);
            discovery::LvmVolumeBuilder::set_storage_pool_capacity_source(volume_ref.get_raw_ref(), pool.get_uuid());
        }
    }
    catch (const std::exception& exception) {
        throw LvmError(exception.what());
    }

    // Refresh Storage Pool capacity info
    DiscoveryManager(context).rediscover<LvmStoragePoolDiscoverer, StoragePool>(
        volume.get_parent_uuid(), update_storage_pool_relations);

}


bool get_fast_flag(const json::Json& value) {
    if (value.is_null()) {
        return false;
    }

    auto type = enums::VolumeInitializationType::from_string(value.get<std::string>());
    switch (type) {
        case enums::VolumeInitializationType::Fast:
            return true;
        case enums::VolumeInitializationType::Slow:
            return false;
        default:
            THROW(InvalidValue, "storage-agent", "Unsupported initialization type.");
    }
}


Uuid initialize_volume_task(agent::storage::SetComponentAttributes::ContextPtr context,
                            const Uuid& uuid, const json::Json& value) {
    log_info("storage-agent", "Executing volume initialization for " << uuid);
    const auto& volume = get_manager<Volume>().get_entry(uuid);

    auto storage_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume.get_uuid());

    if (storage_pools.empty()) {
        THROW(LvmError, "storage-agent", "Storage Pools are empty.");
    }

    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pools.front());

    std::string volume_path{};
    if (storage_pool.get_name().has_value() && volume.get_name().has_value()) {
        volume_path = sysfs::construct_dev_path(storage_pool.get_name(), volume.get_name());
    }
    else {
        THROW(LvmError, "storage-agent", "Storage Pool's and Volume's name must be specified.");
    }

    if (Volume::is_volume_shared_over_fabrics(uuid)) {
        log_error("storage-agent", "Volume " << uuid << " is shared over fabrics.");
        THROW(LvmError, "storage-agent", "Volume is shared over fabrics.");
    }

    bool fast_flag = get_fast_flag(value);
    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();
    task_creator.add_subtask([uuid]() { agent::storage::utils::lock_volume_for_initialization(uuid); });
    task_creator.add_subtask(::storage::lvm::LvmEraseTask{
        volume_path, (std::uint64_t) volume.get_capacity().get_allocated_bytes(), fast_flag
    });

    auto rediscovery_subtask = [context, volume]() {
        DiscoveryManager(context).rediscover<LvmVolumeDiscoverer, Volume>(volume.get_uuid());
        DiscoveryManager(context).rediscover<LvmStoragePoolDiscoverer, StoragePool>(volume.get_parent_uuid(),
            update_storage_pool_relations);
    };

    task_creator.add_subtask(rediscovery_subtask);
    task_creator.add_subtask([uuid]() {
        unlock_volume_for_initialization(uuid);
    });

    task_creator.add_exception_callback([rediscovery_subtask](const agent_framework::exceptions::GamiException&) {
        rediscovery_subtask();
    });
    task_creator.add_exception_callback([uuid](const agent_framework::exceptions::GamiException&) {
        unlock_volume_for_initialization(uuid, false);
    });

    task_creator.set_promised_response([]() { return agent::storage::SetComponentAttributes::Response().to_json(); });

    task_creator.register_task();
    auto initialize_task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(initialize_task);
    return task_creator.get_task_resource().get_uuid();
}
}


void agent::storage::command::process_volume_attributes(AgentContext::SPtr context,
                                                        const Uuid& uuid,
                                                        const attribute::Attributes& attributes,
                                                        SetComponentAttributes::Response& response) {

    StorageValidator::validate_set_volume_attributes(attributes);
    if (attributes.empty()) {
        log_info("storage-agent", "Nothing has been changed (empty request).");
        return;
    }

    const auto attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& field_value = attributes.get_value(attribute_name);

        try {
            if (literals::Volume::BOOTABLE == attribute_name) {
                if (field_value.is_null()) {
                    THROW(UnsupportedField, "storage-agent",
                          "Setting bootable flag to null is not supported.", attribute_name, field_value);
                }
                ::process_bootable_tag(context, uuid, field_value.get<bool>());
            }
            else if (literals::Volume::INITIALIZATION == attribute_name) {
                response.set_task(::initialize_volume_task(context, uuid, field_value));
            }
            else if (literals::Volume::CAPACITY_BYTES == attribute_name) {
                ::process_capacity_bytes(context, uuid, field_value.get<std::int64_t>());
            }
            else {
                THROW(UnsupportedField, "storage-agent",
                      "Setting attribute is not supported.", attribute_name, field_value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}
