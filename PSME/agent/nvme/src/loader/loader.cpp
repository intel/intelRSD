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

    if (!agent_prop.count("vendor") ||
            !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.count("capabilities") ||
           !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.count("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.count("ipv4") ||
            !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.count("port") ||
            !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.count("interval") ||
            !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.count("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.count("port") ||
            !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.count("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }

    if (1 != config["managers"].size()) {
        throw std::runtime_error("'managers' array should have exactly one entry.");
    }

    for(const auto& manager: config["managers"]) {
        if (!manager.count("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.count("nic-drivers") || !manager["nic-drivers"].is_string()) {
            throw std::runtime_error("Each entry in manager must have "
                                     "'nic-drivers' field.");
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
}

void load_manager(const json::Json& json) {
    NvmeConfig::get_instance()->set_ipv4_address(json["ipv4"].get<std::string>());
    NvmeConfig::get_instance()->set_nic_drivers(json["nic-drivers"].get<std::string>());
    NvmeConfig::get_instance()->set_location_offset(json["locationOffset"].get<std::uint16_t>());
    NvmeConfig::get_instance()->set_parent_id(json["parentId"].get<std::string>());
    NvmeConfig::get_instance()->set_rdma_port(json.value("rdma-port", uint16_t{}));
}

void load_managers(const json::Json& config) {
    if (config.count("managers")) {
        const auto& managers_array = config["managers"];

        for (const auto& element : managers_array) {
            load_manager(element);
            // not expecting more managers then one for our agent
            break;
        }
    }
}

}


bool NvmeLoader::load(const json::Json& json) {
    try {
        check_required_fields(json);
        load_managers(json);
    }
    catch (const std::exception& e) {
        log_error("nvme-agent", "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}
