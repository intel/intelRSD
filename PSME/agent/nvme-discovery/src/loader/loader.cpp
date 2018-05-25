/*!
 * @brief Implementation of NvmeLoader class.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file loader.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/fabric.hpp"
#include "agent-framework/module/enum/common.hpp"

#include "loader/loader.hpp"
#include "loader/config.hpp"
#include "logger/logger_factory.hpp"

#include <json/json.hpp>

using namespace agent::nvme::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
void check_required_fields(const json::Value& config) {
    if (!config.is_member("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }
    const auto& agent_prop = config["agent"];
    if (!agent_prop.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent_prop.is_member("vendor") ||
            !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.is_member("capabilities") ||
           !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.is_member("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.is_member("ipv4") ||
            !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.is_member("port") ||
            !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.is_member("interval") ||
            !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.is_member("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.is_member("port") ||
            !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.is_member("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }
    auto& managers_array = config["managers"].as_array();

    if (1 != managers_array.size()) {
        throw std::runtime_error("'managers' array should have exactly one entry.");
    }

    for(const auto& manager: managers_array) {
        if (!manager.is_member("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.is_member("locationOffset") || !manager["locationOffset"].is_number()) {
            throw std::runtime_error("Each entry in manager must have "
                                     "'locationOffset' field.");
        }
        if (!manager.is_member("parentId") || !manager["parentId"].is_string()) {
            throw std::runtime_error("Each entry in manager must have "
                                     "'parentId' field.");
        }
    }

    if (!config.is_member("discovery-service")) {
        throw std::runtime_error("'discovery-service' field is required.");
    }
    const auto& discovery_service = config["discovery-service"];
    if (!discovery_service.is_object()) {
        throw std::runtime_error("'discovery-service' field should be an object");
    }
    if (!discovery_service.is_member("listener-interfaces") || !config["managers"].is_array()) {
        throw std::runtime_error("'listener-interfaces' field is required and should be an array.");
    }
    auto& interfaces_array = discovery_service["listener-interfaces"].as_array();
    for(const auto& iface: interfaces_array) {
        if (!iface.is_member("ofi-provider") || !iface["ofi-provider"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have 'ofi-provider' field.");
        }
        if (!iface.is_member("trtype") || !iface["trtype"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have 'trtype' field.");
        }
        if (!iface.is_member("adrfam") || !iface["adrfam"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                     "'adrfam' field.");
        }
        if (!iface.is_member("traddr") || !iface["traddr"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                     "'traddr' field.");
        }
        if (!iface.is_member("trsvcid") || !iface["trsvcid"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                     "'trsvcid' field.");
        }
    }
}

void load_manager(const json::Value& json) {
    NvmeConfig::get_instance()->set_ipv4_address(json["ipv4"].as_string());
    NvmeConfig::get_instance()->set_location_offset(json["locationOffset"].as_uint());
    NvmeConfig::get_instance()->set_parent_id(json["parentId"].as_string());
}

void load_managers(const json::Value& config) {
    auto& managers_array = config["managers"].as_array();

    for (const auto& element : managers_array) {
        load_manager(element);
    }
}

void load_discovery_service_interfaces(const json::Value& config) {
    const auto& interfaces_array = config["discovery-service"]["listener-interfaces"].as_array();

    nvmf_discovery::Interfaces interfaces{};

    for (const auto& iface : interfaces_array) {
        nvmf_discovery::Interface interface{};
        interface.provider = iface["ofi-provider"].as_string();
        interface.trtype = iface["trtype"].as_string();
        interface.traddr = iface["traddr"].as_string();
        interface.adrfam = iface["adrfam"].as_string();
        interface.trsvcid = iface["trsvcid"].as_string();
        interfaces.push_back(std::move(interface));
    }

    NvmeConfig::get_instance()->set_discovery_service_interfaces(interfaces);
}

}


bool NvmeLoader::load(const json::Value& json) {
    try {
        check_required_fields(json);
        load_managers(json);
        load_discovery_service_interfaces(json);
    }
    catch (const std::exception& e) {
        log_error("nvme-discovery-agent", "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}
