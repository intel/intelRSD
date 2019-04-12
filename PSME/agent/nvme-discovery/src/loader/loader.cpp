/*!
 * @brief Implementation of NvmeLoader class.
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
 * @file loader.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/fabric.hpp"
#include "agent-framework/module/enum/common.hpp"

#include "loader/loader.hpp"
#include "loader/config.hpp"
#include "logger/logger_factory.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent::nvme::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {
void check_required_fields(const json::Json& config) {
    if (!config.count("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }
    const auto& agent_prop = config["agent"];
    if (!agent_prop.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent_prop.count("vendor") || !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.count("capabilities") || !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.count("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.count("ipv4") || !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.count("port") || !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.count("interval") || !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.count("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.count("port") || !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.count("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }
    auto& managers_array = config["managers"];

    if (1 != managers_array.size()) {
        throw std::runtime_error("'managers' array should have exactly one entry.");
    }

    for (const auto& manager: managers_array) {
        if (!manager.count("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.count("locationOffset") || !manager["locationOffset"].is_number()) {
            throw std::runtime_error("Each entry in manager must have "
                                         "'locationOffset' field.");
        }
        if (!manager.count("parentId") || !manager["parentId"].is_string()) {
            throw std::runtime_error("Each entry in manager must have "
                                         "'parentId' field.");
        }
    }

    if (!config.count("discovery-service")) {
        throw std::runtime_error("'discovery-service' field is required.");
    }
    const auto& discovery_service = config["discovery-service"];
    if (!discovery_service.is_object()) {
        throw std::runtime_error("'discovery-service' field should be an object");
    }
    if (!discovery_service.count("listener-interfaces") || !discovery_service["listener-interfaces"].is_array()) {
        throw std::runtime_error("'listener-interfaces' field is required and should be an array.");
    }
    auto& interfaces_array = discovery_service["listener-interfaces"];
    for (const auto& iface: interfaces_array) {
        if (!iface.count("ofi-provider") || !iface["ofi-provider"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have 'ofi-provider' field.");
        }
        if (!iface.count("trtype") || !iface["trtype"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have 'trtype' field.");
        }
        if (!iface.count("adrfam") || !iface["adrfam"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                         "'adrfam' field.");
        }
        if (!iface.count("traddr") || !iface["traddr"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                         "'traddr' field.");
        }
        if (!iface.count("trsvcid") || !iface["trsvcid"].is_string()) {
            throw std::runtime_error("Each entry in interfaces must have "
                                         "'trsvcid' field.");
        }
    }
}


void load_manager(const json::Json& json) {
    NvmeConfig::get_instance()->set_ipv4_address(json["ipv4"]);
    NvmeConfig::get_instance()->set_location_offset(json["locationOffset"]);
    NvmeConfig::get_instance()->set_parent_id(json["parentId"]);
}


void load_managers(const json::Json& config) {
    if (config.count("managers")) {
        auto& managers_array = config["managers"];

        for (const auto& element : managers_array) {
            load_manager(element);
        }
    }
}


void load_discovery_service_interfaces(const json::Json& config) {
    nvmf_discovery::Interfaces interfaces{};

    if (config.count("discovery-service") && config["discovery-service"].count("listener-interfaces")) {
        const auto& interfaces_array = config["discovery-service"]["listener-interfaces"];

        for (const auto& iface : interfaces_array) {
            nvmf_discovery::Interface interface{};
            interface.provider = iface["ofi-provider"].get<std::string>();
            interface.trtype = iface["trtype"];
            interface.traddr = iface["traddr"];
            interface.adrfam = iface["adrfam"];
            interface.trsvcid = iface["trsvcid"];
            interfaces.push_back(std::move(interface));
        }
    }

    NvmeConfig::get_instance()->set_discovery_service_interfaces(interfaces);
}

}


bool NvmeLoader::load(const json::Json& json) {
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
