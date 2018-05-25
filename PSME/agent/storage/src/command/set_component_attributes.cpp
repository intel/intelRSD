/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * */


#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "command/set_endpoint_attributes.hpp"
#include "command/set_volume_attributes.hpp"
#include "command/set_chassis_attributes.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::command;

namespace {

bool exists_in_storage(const Uuid& uuid) {
    return get_manager<Manager>().entry_exists(uuid)
           || get_manager<StorageService>().entry_exists(uuid)
           || get_manager<StoragePool>().entry_exists(uuid)
           || get_manager<Fabric>().entry_exists(uuid)
           || get_manager<Zone>().entry_exists(uuid)
           || get_manager<Drive>().entry_exists(uuid);
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {

    log_debug("storage-agent", "Trying to execute SetComponentAttributes method.");
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    if (get_manager<Volume>().entry_exists(uuid)) {
        agent::storage::process_volume_attributes(uuid, attributes, response);
    }
    else if (get_manager<Endpoint>().entry_exists(uuid)) {
        agent::storage::process_endpoint_attributes(uuid, attributes, response);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        agent::storage::process_chassis_attributes(uuid, attributes, response);
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

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
