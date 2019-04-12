/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file storage_loader.cpp
 * */

#include "loader/storage_loader.hpp"

#include "json-wrapper/json-wrapper.hpp"
#include "logger/logger.hpp"



using namespace agent::storage::loader;

namespace {

void check_required_fields(const json::Json& config) {
    if (!config.count("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }

    const auto& agent = config["agent"];
    if (!agent.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent.count("vendor") || !agent["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent.count("capabilities") || !agent["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.count("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }

    const auto& registration = config["registration"];
    if (!registration.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration.count("ipv4") || !registration["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration.count("port") || !registration["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration.count("interval") || !registration["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.count("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server = config["server"];
    if (!server.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server.count("port") || !server["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.count("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }

    if (1 != config["managers"].size()) {
        throw std::runtime_error("'managers' array should have exactly one entry.");
    }

    for (const auto& manager : config["managers"]) {
        if (!manager.count("rdma-port") || !manager["rdma-port"].is_number()) {
            throw std::runtime_error("Each entry in manager must have 'rdma-port' field with type: number.");
        }
        if (!manager.count("nic-drivers") || !manager["nic-drivers"].is_array()) {
            throw std::runtime_error("Each entry in manager must have 'nic-drivers' field with type: array.");
        }
        for (const auto& driver : manager["nic-drivers"]) {
            if (!driver.is_string()) {
                throw std::runtime_error("Each driver in nic-drivers must be a string.");
            }
        }
    }
}

void load_managers(std::shared_ptr<AgentConfiguration> configuration, const json::Json& json) {
    for (const auto& manager : json["managers"]) {
        std::vector<std::string> drivers{};
        for (const auto& driver : manager["nic-drivers"]) {
            drivers.push_back(driver.get<std::string>());
        }
        configuration->set_nic_drivers(drivers);
        configuration->set_rdma_port(manager["rdma-port"].get<std::uint16_t>());

        // Expected only one manager entry in the configuration file
        break;
    }
}

}


StorageLoader::StorageLoader() : m_configuration(std::make_shared<AgentConfiguration>()) {
}


bool StorageLoader::load(const json::Json& json) {
    try {
        check_required_fields(json);
        load_managers(m_configuration, json);
    }
    catch (const std::exception& e) {
        log_error("storage-agent", "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}


std::shared_ptr<AgentConfiguration> StorageLoader::get() const {
    return m_configuration;
}