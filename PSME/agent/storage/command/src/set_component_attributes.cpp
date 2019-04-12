/*!
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file set_component_attributes.cpp
 * @brief Set Component Attributes command
 */


#include "agent-framework/module/storage_components.hpp"

#include "agent/command/set_volume_attributes.hpp"
#include "agent/command/set_chassis_attributes.hpp"
#include "agent/command/set_endpoint_attributes.hpp"
#include "storage_agent_commands.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::storage;

namespace {

bool exists_in_storage(const Uuid& uuid) {
    return get_manager<Manager>().entry_exists(uuid)
           || get_manager<StorageService>().entry_exists(uuid)
           || get_manager<StoragePool>().entry_exists(uuid)
           || get_manager<Fabric>().entry_exists(uuid)
           || get_manager<Zone>().entry_exists(uuid)
           || get_manager<Drive>().entry_exists(uuid);
}


void set_component_attributes(SetComponentAttributes::ContextPtr context,
                              const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {

    log_debug("storage-agent", "Trying to execute SetComponentAttributes method.");
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (get_manager<Volume>().entry_exists(uuid)) {
        agent::storage::command::process_volume_attributes(context, uuid, attributes, response);
    }
    else if (get_manager<Endpoint>().entry_exists(uuid)) {
        agent::storage::command::process_endpoint_attributes(context, uuid, attributes, response);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        agent::storage::command::process_chassis_attributes(uuid, attributes, response);
    }
    else if (exists_in_storage(uuid)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Operation not available for this component.");
    }
    else {
        THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
              "No component with UUID = '" + uuid + "'.");
    }

    log_info("storage-agent", "setComponentAttributes finished successfully.");
}

}

REGISTER_STORAGE_COMMAND(SetComponentAttributes, ::set_component_attributes);
