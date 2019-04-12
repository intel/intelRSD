/*!
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
 * @file set_volume_attributes.cpp
 */

#include "command/set_volume_attributes.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"



using namespace agent::nvme;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;

namespace {

bool get_fast_flag(const std::string& value) {
    auto type = enums::VolumeInitializationType::from_string(value);
    switch (type) {
        case enums::VolumeInitializationType::Fast:
            return true;
        case enums::VolumeInitializationType::Slow:
            return false;
        default:
            THROW(InvalidValue, "nvme-agent", "Unsupported initialization type.");
    }
}


void throw_if_volume_is_shared(const Uuid& uuid) {
    if (tools::is_volume_shared(uuid)) {
        log_error("nvme-agent", "Volume " << uuid << " is shared over fabrics");
        THROW(NvmeError, "nvme-agent", "Volume is shared over fabrics");
    }
}


std::tuple<std::string, std::string> get_drive_and_volume_name(const Uuid& uuid) {
    auto volume = get_manager<Volume>().get_entry(uuid);
    auto storage_pool_uuids = get_m2m_manager<StoragePool, Volume>().get_parents(uuid);
    if (storage_pool_uuids.size() != 1) {
        THROW(NvmeError, "nvme-agent",
            "Invalid number of storage pools for volume: " + std::to_string(storage_pool_uuids.size()));
    }
    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pool_uuids.front());
    if (!storage_pool.get_name().has_value()) {
        log_error("nvme-agent", "Storage pool " << storage_pool.get_uuid() << " has no name!");
        throw NvmeError("Related Storage Pool has no name!");
    }
    return std::make_tuple(storage_pool.get_name(), volume.get_name());
}


void lock_volume(const Uuid& uuid) {
    auto volume_ref = get_manager<Volume>().get_entry_reference(uuid);
    if (volume_ref->get_is_being_initialized()) {
        throw std::runtime_error("Volume is already being initialized!");
    }
    else {
        volume_ref->set_is_being_initialized(true);
        volume_ref->set_status(attribute::Status{enums::State::Starting, enums::Health::OK});
    }
}


void unlock_and_handle_fail(const Uuid& uuid) {
    tools::VolumeDatabase db{uuid};
    db.put(tools::NvmeDatabase::VOLUME_ERASED, json::Json().dump());
    {
        auto volume_ref = get_manager<Volume>().get_entry_reference(uuid);
        volume_ref->set_is_being_initialized(false);
        volume_ref->set_status(attribute::Status{enums::State::Enabled, enums::Health::Warning});
    }
}


void unlock_and_handle_success(const Uuid& uuid) {
    tools::VolumeDatabase db{uuid};
    db.put(tools::NvmeDatabase::VOLUME_ERASED, json::Json(true).dump());
    {
        auto volume_ref = get_manager<Volume>().get_entry_reference(uuid);
        volume_ref->set_is_being_initialized(false);
        volume_ref->set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    }
}


void initialize_volume(SetComponentAttributes::ContextPtr context, const Uuid& uuid, bool fast_flag) {
    std::string drive_name{};
    std::string volume_name{};
    std::tie(drive_name, volume_name) = get_drive_and_volume_name(uuid);

    try {
        // prepare handler
        auto handler = context->drive_handler_factory->get_handler(drive_name);
        if (!handler) {
            throw std::runtime_error(std::string{"Unable to get handler for drive "} + drive_name);
        }
        handler->load();

        // initialize
        handler->erase_volume(volume_name, fast_flag);
    }
    catch (const std::exception& exception) {
        THROW(NvmeError, "nvme-agent", exception.what());
    }
}


Uuid initialize_volume_task(SetComponentAttributes::ContextPtr context, const Uuid& uuid, const std::string& value) {
    log_info("nvme-agent", "Executing volume initialization.");

    throw_if_volume_is_shared(uuid);
    bool fast_flag = get_fast_flag(value);
    action::TaskCreator task_creator{};
    task_creator.prepare_task();
    task_creator.add_subtask([uuid]() { lock_volume(uuid); });
    task_creator.add_subtask([uuid, fast_flag, context]() { initialize_volume(context, uuid, fast_flag); });
    task_creator.add_subtask([uuid]() { unlock_and_handle_success(uuid); });
    task_creator.add_exception_callback([uuid](const exceptions::GamiException&) { unlock_and_handle_fail(uuid); });
    task_creator.set_promised_response([]() { return SetComponentAttributes::Response().to_json(); });

    task_creator.register_task();
    auto initialize_task = task_creator.get_task();
    action::TaskRunner::get_instance().run(initialize_task);
    return task_creator.get_task_resource().get_uuid();
}

}


void agent::nvme::command::set_volume_attributes(SetComponentAttributes::ContextPtr context,
                                    const Uuid& uuid,
                                    const attribute::Attributes& attributes,
                                    SetComponentAttributes::Response& response) {

    log_debug("nvme-agent", "Executing setVolumeAttributes command");
    log_debug("nvme-agent", "Processing Volume [UUID = " << uuid << "]");
    requests::validation::StorageValidator::validate_set_volume_attributes(attributes);

    for (const auto& attribute_name : attributes.get_names()) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug("nvme-agent", "Attribute name: " << attribute_name);
        log_debug("nvme-agent", "Attribute value: " << value.dump());

        if (literals::Volume::INITIALIZATION == attribute_name) {
            try {
                response.set_task(initialize_volume_task(context, uuid, value.get<std::string>()));
            }
            catch (const exceptions::GamiException&) {
                throw;
            }
            catch (const std::exception& e) {
                THROW(NvmeError, "nvme-agent", std::string{"Error while initializing volume: "} + e.what());
            }
        }
        else {
            // GeoTag, Reset, LocationId, Oem, all these fields are not supported
            THROW(UnsupportedField, "nvme-agent", "Setting attribute is not supported.", attribute_name, value);
        }
    }
}
