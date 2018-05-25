/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "loader/compute_loader.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::compute::loader;
using namespace agent::compute;
using namespace std;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

using agent_framework::module::CommonComponents;

namespace {
void check_required_fields(const json::Value& config) {
    if (!config["agent"].is_object()) {
        throw std::runtime_error("'agent' field is required.");
    }

    if (!config["agent"]["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required.");
    }

    if (!config["agent"]["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required.");
    }

    if (!config["registration"].is_object()) {
        throw std::runtime_error("'registration' field is required.");
    }

    if (!config["registration"]["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required.");
    }

    if (!config["registration"]["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required.");
    }

    if (!config["registration"]["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required.");
    }

    if (!config["server"].is_object()) {
        throw std::runtime_error("'server' field is required.");
    }

    if (!config["server"]["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required.");
    }

    if (!config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required.");
    }

    auto& managers_array = config["managers"].as_array();

    if (managers_array.empty()) {
        throw std::runtime_error("'managers' array should have at least one entry.");
    }

    for (const auto& manager: managers_array) {
        if (!manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have ipv4 field.");
        }
        if (!manager["username"].is_string()) {
            throw std::runtime_error("Each entry in manager must have username field.");
        }
        if (!manager["password"].is_string()) {
            throw std::runtime_error("Each entry in manager must have password field.");
        }
        if (!manager["port"].is_number()) {
            throw std::runtime_error("Each entry in manager must have port field.");
        }
        if (!manager["slot"].is_number()) {
            throw std::runtime_error("Each entry in manager must have slot field.");
        }
    }
}
}


bool ComputeLoader::load(const json::Value& config) {
    try {
        check_required_fields(config);
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Loading modules configuration failed: " << error.what() << std::endl;
        return false;
    }

    return true;
}
