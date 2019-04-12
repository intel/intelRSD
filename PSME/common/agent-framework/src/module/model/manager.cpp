/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @file manager.cpp
 * @brief manager
 * */

#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Manager::component = enums::Component::Manager;
const enums::CollectionName Manager::collection_name = enums::CollectionName::Managers;

Manager::Manager(const std::string& parent_uuid, enums::Component parent_type) : Resource(parent_uuid, parent_type) {}

Manager::~Manager() {}

json::Json Manager::to_json() const {
    json::Json result = json::Json();
    result[literals::Manager::STATUS] = get_status().to_json();
    result[literals::Manager::TYPE] = get_manager_type();
    result[literals::Manager::MODEL] = get_manager_model();
    result[literals::Manager::LOCATION] = get_location();
    result[literals::Manager::DATE_TIME] = get_date_time();
    result[literals::Manager::DATE_TIME_LOCAL_OFFSET] = get_date_time_local_offset();
    result[literals::Manager::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Manager::IPv4_ADDRESS] = get_ipv4_address();
    result[literals::Manager::NETWORK_SERVICES] = get_network_services().to_json();
    result[literals::Manager::GRAPHICAL_CONSOLE] = get_graphical_console().to_json();
    result[literals::Manager::SERIAL_CONSOLE] = get_serial_console().to_json();
    result[literals::Manager::COMMAND_SHELL] = get_command_shell().to_json();
    result[literals::Manager::COLLECTIONS] = get_collections().to_json();
    result[literals::Manager::ALLOWED_ACTIONS] = get_allowed_reset_actions().to_json();
    result[literals::Manager::OEM] = get_oem().to_json();
    result[literals::Manager::PARENT_ID] = get_parent_uuid();
    result[literals::Manager::GUID] = get_guid();
    return result;
}

Manager Manager::from_json(const json::Json& json) {
    Manager manager;

    manager.set_status(attribute::Status::from_json(json[literals::Manager::STATUS]));
    manager.set_manager_type(json[literals::Manager::TYPE]);
    manager.set_manager_model(json[literals::Manager::MODEL]);
    manager.set_location(json[literals::Manager::LOCATION]);
    manager.set_date_time(json[literals::Manager::DATE_TIME]);
    manager.set_date_time_local_offset(json[literals::Manager::DATE_TIME_LOCAL_OFFSET]);
    manager.set_firmware_version(json[literals::Manager::FIRMWARE_VERSION]);
    manager.set_ipv4_address(json[literals::Manager::IPv4_ADDRESS]);
    manager.set_network_services(NetworkServices::from_json(json[literals::Manager::NETWORK_SERVICES]));
    manager.set_graphical_console(attribute::GraphicalConsole::from_json(json[literals::Manager::GRAPHICAL_CONSOLE]));
    manager.set_serial_console(attribute::SerialConsole::from_json(json[literals::Manager::SERIAL_CONSOLE]));
    manager.set_command_shell(attribute::CommandShell::from_json(json[literals::Manager::COMMAND_SHELL]));
    manager.set_collections(Collections::from_json(json[literals::Manager::COLLECTIONS]));
    manager.set_allowed_reset_actions(attribute::Array<enums::ResetType>::from_json(json[literals::Manager::ALLOWED_ACTIONS]));
    manager.set_oem(attribute::Oem::from_json(json[literals::Manager::OEM]));
    manager.set_parent_uuid(json[literals::Manager::PARENT_ID]);
    manager.set_guid(json[literals::Manager::GUID]);

    return manager;
}
