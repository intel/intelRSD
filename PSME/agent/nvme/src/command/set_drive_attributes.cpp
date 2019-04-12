/*!
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file set_drive_attributes.cpp
 */

#include "command/set_drive_attributes.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"



using namespace agent::nvme;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;

namespace {
void update_latency_tracking(agent::nvme::SetComponentAttributes::ContextPtr context,
                             const Uuid& uuid, const json::Json& value) {

    auto drive = get_manager<Drive>().get_entry(uuid);

    if (drive.get_name().has_value()) {

        auto handler = context->drive_handler_factory->get_handler(drive.get_name());
        if (!handler) {
            throw std::runtime_error(
                std::string{"Unable to get handler for drive "} + drive.get_name().value());
        }
        handler->load();

        std::uint32_t namespace_id{};
        if (!handler->get_drive_data().namespaces.empty()) {
            namespace_id = handler->get_drive_data().namespaces.front();
        }
        else {
            throw NvmeError("Related Drive has no namespaces!");
        }

        if (value.get<bool>()) {
            context->nvme_interface->enable_latency_tracking_feature(drive.get_name(), namespace_id);
            log_debug("nvme-agent",
                      "Latency Tracking Enabled for " << drive.get_name() << " drive with namespace ID "
                                                      << namespace_id);
        }
        else {
            context->nvme_interface->disable_latency_tracking_feature(drive.get_name(), namespace_id);
            log_debug("nvme-agent",
                      "Latency Tracking Disabled for " << drive.get_name()
                                                       << " drive with namespace ID "
                                                       << namespace_id);
        }

        {
            auto drive_ref = get_manager<Drive>().get_entry_reference(uuid);
            auto result = context->nvme_interface->get_latency_tracking_feature(drive.get_name(),
                                                                                namespace_id);
            drive_ref->set_latency_tracking_enabled(bool(result));
        }
    }
    else {
        log_error("nvme-agent", "Drive " << drive.get_uuid() << " has no name!");
        throw NvmeError("Related Drive has no name!");
    }
}
}


void
agent::nvme::command::set_drive_attributes(agent::nvme::SetComponentAttributes::ContextPtr context, const Uuid& uuid,
                                           const attribute::Attributes& attributes,
                                           SetComponentAttributes::Response& response) {

    log_info("nvme-agent", "Executing setDriveAttributes command");
    log_debug("nvme-agent", "Processing Drive [UUID = " << uuid << "]");
    requests::validation::StorageValidator::validate_set_drive_attributes(attributes);

    for (const auto& attribute_name : attributes.get_names()) {
        const auto& value = attributes.get_value(attribute_name);

        try {
            log_debug("nvme-agent", "Attribute name: " << attribute_name);
            log_debug("nvme-agent", "Attribute value: " << value.dump());

            if (literals::Drive::LATENCY_TRACKING_ENABLED == attribute_name) {
                ::update_latency_tracking(context, uuid, value);
            }
            else {
                THROW(UnsupportedField, "nvme-agent", "Setting attribute is not supported.", attribute_name, value);
            }
        }
        catch (const GamiException& e) {
            response.add_status({attribute_name, e.get_error_code(), e.get_message()});
        }

    }
}