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
 *
 * */

#include "loader/storage_loader.hpp"
#include "agent-framework/module-ref/storage_manager.hpp"
#include "agent-framework/module-ref/model/attributes/status.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "logger/logger_factory.hpp"
#include "utils/net_utils.hpp"
#include "json/json.hpp"

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>

using std::string;
using namespace agent::storage::loader;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

bool StorageLoader::read_modules(const json::Value& json) {
    if (json.is_member("modules") && json["modules"].is_array()) {
        for (const auto& module_json : json["modules"].as_array()) {
            if (module_json.is_object()) {
                return make_manager_module(module_json);
            }
        }
    }
    log_error(GET_LOGGER("storage-agent"), "Modules are not configured");
    return false;
}

bool StorageLoader::make_manager_module(const json::Value& json) {
    if (json.is_member("ipv4") && json["ipv4"].is_string()) {
        Manager manager{};
        manager.set_status({enums::State::Enabled, enums::Health::OK});
        manager.set_ipv4_address(json["ipv4"].as_string());
        if (json.is_member("submodules") && json["submodules"].is_array()) {
            for (const auto& submodule : json["submodules"].as_array()) {
                make_submodule(submodule, manager.get_uuid());
            }
        }
        log_debug(GET_LOGGER("storage-agent"), "Created Manager module [ipv4="
            << manager.get_ipv4_address() << "]");
        StorageManager::get_instance()->
                            get_module_manager().add_entry(manager);
        return true;
    }
    return false;
}

bool StorageLoader::make_submodule(const json::Value& json,
    const std::string& uuid) {
    Component storage{Component::StorageServices};
    if (json.is_member("type") && json["type"].is_string() &&
        storage.to_string() == json["type"].as_string()) {
        StorageServices storage_services{uuid};
        /* read iscsi configuration */
        if (json.is_member("iscsi") && json["iscsi"].is_object()) {
            IscsiData iscsi_data{};
            auto iscsi_json = json["iscsi"];
            if (iscsi_json.is_member("portal-interface") &&
                                iscsi_json["portal-interface"].is_string()) {
                iscsi_data.set_interface_name(
                        iscsi_json["portal-interface"].as_string());
                iscsi_data.set_portal_ip(utils::NetUtils::get_ip_address(
                            iscsi_json["portal-interface"].as_string()));
            }
            if (iscsi_json.is_member("port") && iscsi_json["port"].is_uint()) {
                iscsi_data.set_portal_port(iscsi_json["port"].as_uint());
            }
            if (iscsi_json.is_member("username") &&
                                        iscsi_json["username"].is_string()) {
                iscsi_data.set_username(iscsi_json["username"].as_string());
            }
            if (iscsi_json.is_member("password") &&
                                        iscsi_json["password"].is_string()) {
                iscsi_data.set_password(iscsi_json["password"].as_string());
            }
            if (iscsi_json.is_member("initiator") &&
                                        iscsi_json["initiator"].is_string()) {
                iscsi_data.set_initiator(iscsi_json["initiator"].as_string());
            }
            if (iscsi_json.is_member("config-path") &&
                                    iscsi_json["config-path"].is_string()) {
                iscsi_data.set_configuration_path(
                                    iscsi_json["config-path"].as_string());
            }
            auto& iscsi_manager = get_manager<IscsiTarget, IscsiTargetManager>();
            iscsi_manager.set_iscsi_data(std::move(iscsi_data));
        }
        /* add StorageServices model to the manager */
        get_manager<StorageServices>().add_entry(storage_services);
    }
    return true;
}

bool StorageLoader::load(const json::Value& json) {
    try {
        /* read storage configuration */
        return read_modules(json);
    }
    catch (const json::Value::Exception& e) {
        log_error(GET_LOGGER("storage-agent"),
                "Load module configuration failed: " << e.what());
    }
    return false;
}
