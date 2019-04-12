/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file set_chassis_attributes.cpp
 * @brief Set Chassis Attributes command
 */

#include "agent-framework/module/requests/validation/common.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"

#include "command/set_chassis_attributes.hpp"

using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::requests::validation;

namespace {

void set_asset_tag(const Uuid& uuid, const json::Json& value) {
    static std::mutex local_mutex{};
    std::lock_guard<std::mutex> lock(local_mutex);

    log_debug("storage-agent", "Setting assetTag for chassis " << uuid);
    {
        auto chassis_ref = get_manager<Chassis>().get_entry_reference(uuid);

        agent_framework::database::ChassisEntity entity{uuid};
        entity.put(agent_framework::database::CHASSIS_ASSET_TAG_PROPERTY, value);

        chassis_ref->set_asset_tag(value);
    }
    log_info("storage-agent", "Attribute 'assetTag' has been set to: " << value.dump());
}

}

namespace agent {
namespace spdk {
namespace command {

void set_chassis_attributes(AgentContext::SPtr, const Uuid& uuid, const attribute::Attributes& attributes,
                            SetComponentAttributes::Response& response) {

    log_info("spdk-agent", "Processing SetChassisAttributes [UUID = " << uuid << "]");
    CommonValidator::validate_set_chassis_attributes(attributes);

    const auto attribute_names = attributes.get_names();
    if (attribute_names.empty()) {
        log_debug("spdk-agent", "setComponentAttributes: nothing has been changed (empty request).");
        return;
    }

    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        log_debug("spdk-agent", "Attribute name: " << name);
        log_debug("spdk-agent", "Attribute value: " << value.dump());

        try {
            if (literals::Chassis::ASSET_TAG == name) {
                ::set_asset_tag(uuid, value);
            }
            else {
                // The response must have a message for every attribute that could not be set.
                THROW(UnsupportedField, "spdk-agent", "Setting attribute is not supported.", name, value);
            }
        }
        catch (const GamiException& ex) {
            response.add_status({name, ex.get_error_code(), ex.get_message()});
        }
    }
}

}
}
}
