/*!
 * @brief Implementation of SetComponentAttributes command.
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
 * @file set_component_attributes.cpp
 */

#include "nvme_agent_commands.hpp"
#include "command/set_chassis_attributes.hpp"
#include "command/set_volume_attributes.hpp"
#include "command/set_drive_attributes.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::nvme;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;

namespace {

bool resource_is_unchangeable(const Uuid& uuid) {
    return get_manager<Manager>().entry_exists(uuid) ||
           get_manager<StorageService>().entry_exists(uuid) ||
           get_manager<StoragePool>().entry_exists(uuid) ||
           get_manager<NetworkInterface>().entry_exists(uuid) ||
           get_manager<Fabric>().entry_exists(uuid) ||
           get_manager<Zone>().entry_exists(uuid) ||
           get_manager<Endpoint>().entry_exists(uuid) ||
           get_manager<System>().entry_exists(uuid);
}


void set_component_attributes(SetComponentAttributes::ContextPtr context, const SetComponentAttributes::Request& req,
                              SetComponentAttributes::Response& res) {
    log_debug("nvme-agent", "Executing setComponentAttributes.");
    const auto& uuid = req.get_component();
    const auto& attributes = req.get_attributes();

    if (attributes.empty()) {
        log_debug("nvme-agent", "Nothing has been changed (empty request).");
        return;
    }

    if (get_manager<Volume>().entry_exists(uuid)) {
        command::set_volume_attributes(context, uuid, attributes, res);
    }
    else if (get_manager<Chassis>().entry_exists(uuid)) {
        command::set_chassis_attributes(context, uuid, attributes, res);
    }
    else if (get_manager<Drive>().entry_exists(uuid)) {
        command::set_drive_attributes(context, uuid, attributes, res);
    }
    else if (resource_is_unchangeable(uuid)) {
        THROW(InvalidValue, "nvme-agent", "Operation not available for this component.");
    }
    else {
        THROW(InvalidUuid, "nvme-agent", "Component not found.");
    }
    log_debug("nvme-agent", "setComponentAttributes finished successfully.");
}

}

REGISTER_NVME_COMMAND(SetComponentAttributes, ::set_component_attributes);
