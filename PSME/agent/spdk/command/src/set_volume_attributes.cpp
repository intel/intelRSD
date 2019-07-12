/*!
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
 * @file set_volume_attributes.cpp
 */


#include "spdk_converter.hpp"
#include "spdk_stabilizer.hpp"
#include "spdk_agent_context.hpp"
#include "discovery/builders/spdk_volume_builder.hpp"
#include "discovery/builders/spdk_storage_pool_builder.hpp"

#include "command/set_volume_attributes.hpp"
#include "command/helpers/helpers.hpp"
#include "discovery/builders/spdk_volume_builder.hpp"

#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"



using namespace agent_framework;
using namespace agent::spdk::discovery;

namespace {

void initialize_volume(agent::spdk::AgentContext::SPtr context, const Uuid& volume_uuid) {

    auto volume = module::get_manager<model::Volume>().get_entry(volume_uuid);
    auto pools = module::get_m2m_manager<model::StoragePool, model::Volume>().get_parents(volume_uuid);

    if (pools.empty()) {
        THROW(exceptions::SpdkError, "spdk-agent", "Volume is not assigned to storage pool!");
    }
    auto pool = module::get_manager<model::StoragePool>().get_entry(pools.front());
    auto bdev_name = agent::spdk::utils::SpdkConverter::make_alias(
        pool.get_name(), model::attribute::Identifier::get_uuid(volume));

    try {
        context->spdk_api->resize_lvol_bdev(bdev_name, 0);
        context->spdk_api->resize_lvol_bdev(bdev_name, volume.get_capacity().get_allocated_bytes());
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent", "Could not resize volume: " + std::string{exception.what()});
    }

    try {
        auto bdev = context->spdk_api->get_bdevs(bdev_name).front();
        agent::spdk::discovery::SpdkVolumeBuilder::update_with_spdk(volume, bdev);
        module::get_manager<model::Volume>().add_or_update_entry(volume);
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent",
              "Could not refresh Volume data: " + std::string{exception.what()});
    }

    agent::spdk::command::refresh_storage_pool(context, pool.get_uuid());
}

void process_capacity_bytes(agent::spdk::AgentContext::SPtr context,
                            const Uuid& volume_uuid,
                            std::int64_t requested_capacity) {

    const auto& volume = module::get_manager<model::Volume>().get_entry(volume_uuid);
    auto pools = module::get_m2m_manager<model::StoragePool, model::Volume>().get_parents(volume_uuid);
    if (pools.empty()) {
        THROW(exceptions::SpdkError, "spdk-agent", "Volume is not assigned to storage pool!");
    }
    auto pool = module::get_manager<model::StoragePool>().get_entry(pools.front());

    std::int64_t actual_capacity = volume.get_capacity().get_allocated_bytes();
    log_info("spdk-agent", "Request to resize volume from " << actual_capacity << " to " << requested_capacity);
    if (requested_capacity < actual_capacity) {
        THROW(exceptions::UnsupportedValue, "spdk-agent", "Only volume extension is supported.");
    }
    if (!model::StoragePool::can_allocate_bytes(pool, requested_capacity)) {
        THROW(exceptions::InvalidValue, "spdk-agent",
            "Requested capacity to resize exceeds available capacity of pool.");
    }

    if (model::Volume::is_volume_shared_over_fabrics(volume_uuid)) {
        THROW(exceptions::InvalidValue, "spdk-agent",
            "Cannot resize volume which is shared over fabrics. Remove related endpoints from zone first.");
    }

    auto bdev_name = agent::spdk::utils::SpdkConverter::make_alias(
        pool.get_name(), model::attribute::Identifier::get_uuid(volume));

    std::lock_guard<std::mutex> lock{context->spdk_mutex};
    bool success{};
    try {
        success = context->spdk_api->resize_lvol_bdev(bdev_name, static_cast<std::uint64_t>(requested_capacity));
    }
    catch(const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent", "Could not resize volume: " + std::string{exception.what()});
    }

    if (!success) {
        THROW(exceptions::SpdkError, "spdk-agent", "Failed to resize Logical Volume!");
    }

    try {
        auto updated_bdev = context->spdk_api->get_bdevs(bdev_name).at(0);
        auto volume_ref = module::get_manager<model::Volume>().get_entry_reference(volume_uuid);
        agent::spdk::discovery::SpdkVolumeBuilder::update_with_spdk(volume_ref.get_raw_ref(), updated_bdev);
        agent::spdk::discovery::SpdkVolumeBuilder::set_storage_pool_capacity_source(volume_ref.get_raw_ref(), pool.get_uuid());
    }
    catch (const std::exception& exception) {
        log_error("spdk-agent", "SetComponentAttributes failed: " << exception.what());
        THROW(exceptions::SpdkError, "spdk-agent", "Failed to update Volume after resizing!");
    }

    agent::spdk::command::refresh_storage_pool(context, pool.get_uuid());
}

}

namespace agent {
namespace spdk {
namespace command {

void set_volume_attributes(agent::spdk::SetComponentAttributes::ContextPtr context,
                           const Uuid& uuid,
                           const model::attribute::Attributes& attributes,
                           agent::spdk::SetComponentAttributes::Response& response) {

    model::requests::validation::StorageValidator::validate_set_volume_attributes(attributes);
    if (attributes.empty()) {
        log_info("spdk-agent", "Nothing has been changed (empty request).");
        return;
    }

    auto attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& field_value = attributes.get_value(attribute_name);

        try {
            if (agent_framework::model::literals::Volume::INITIALIZATION == attribute_name) {
                ::initialize_volume(context, uuid);
            }
            else if (agent_framework::model::literals::Volume::CAPACITY_BYTES == attribute_name) {
                const auto& value = attributes.get_value(attribute_name);
                ::process_capacity_bytes(context, uuid, value.get<std::int64_t>());
            }
            else {
                THROW(exceptions::UnsupportedField, "spdk-agent",
                      "Setting attribute is not supported.", attribute_name, field_value);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

}
}
}

