/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 * @file manager.cpp
 * @brief manager
 *
 * */

#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/constants/common.hpp"


using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Manager::component = enums::Component::Manager;
const enums::CollectionName Manager::collection_name =
    enums::CollectionName::Managers;

Manager::Manager(const std::string& parent) : Resource{parent} {}

Manager::~Manager() {}

Json::Value Manager::to_json() const {
    Json::Value result;
    result[literals::Manager::STATUS] = get_status().to_json();
    result[literals::Manager::TYPE] = get_manager_type();
    result[literals::Manager::MODEL] = get_manager_model();
    result[literals::Manager::LOCATION] = get_location();
    result[literals::Manager::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Manager::IPv4_ADDRESS] = get_ipv4_address();
    result[literals::Manager::NETWORK_SERVICES] =
        get_network_services().to_json();
    result[literals::Manager::GRAPHICAL_CONSOLE] =
        get_graphical_console().to_json();
    result[literals::Manager::SERIAL_CONSOLE] = get_serial_console().to_json();
    result[literals::Manager::COMMAND_SHELL] = get_command_shell().to_json();
    result[literals::Manager::COLLECTIONS] = get_collections().to_json();
    result[literals::Manager::OEM] = get_oem().to_json();
    result[literals::Manager::PARENT_ID] = get_parent_uuid();
    result[literals::Manager::GUID] = get_guid();
    return result;
}

Manager Manager::from_json(const Json::Value& json) {
    Manager manager;

    manager.set_status(attribute::Status::from_json(json[literals::Manager::STATUS]));
    manager.set_manager_type(json[literals::Manager::TYPE]);
    manager.set_manager_model(json[literals::Manager::MODEL]);
    manager.set_location(json[literals::Manager::LOCATION]);
    manager.set_firmware_version(
        json[literals::Manager::FIRMWARE_VERSION]);
    manager.set_ipv4_address(
        json[literals::Manager::IPv4_ADDRESS].asString());
    manager.set_network_services(NetworkServices::from_json(
        json[literals::Manager::NETWORK_SERVICES]));
    manager.set_graphical_console(attribute::GraphicalConsole::from_json(
        json[literals::Manager::GRAPHICAL_CONSOLE]));
    manager.set_serial_console(attribute::SerialConsole::from_json(
        json[literals::Manager::SERIAL_CONSOLE]));
    manager.set_command_shell(attribute::CommandShell::from_json(
        json[literals::Manager::COMMAND_SHELL]));
    manager.set_collections(Collections::from_json(
        json[literals::Manager::COLLECTIONS]));
    manager.set_oem(attribute::Oem::from_json(json[literals::Manager::OEM]));
    manager.set_resource_hash(compute_hash(json));
    manager.set_parent_uuid(json[literals::Manager::PARENT_ID].asString());
    manager.set_guid(json[literals::Manager::GUID]);

    return manager;
}
