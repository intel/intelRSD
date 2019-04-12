/*!
 * @section LICENSE
 *
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
void check_required_fields(const json::Json& config) {
    if (!config.value("agent", json::Json()).is_object()) {
        throw std::runtime_error("'agent' field is required.");
    }

    if (!config.value("agent", json::Json::object()).value("vendor", json::Json()).is_string()) {
        throw std::runtime_error("'agent:vendor' field is required.");
    }

    if (!config.value("agent", json::Json::object()).value("capabilities", json::Json()).is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required.");
    }

    if (!config.value("registration", json::Json()).is_object()) {
        throw std::runtime_error("'registration' field is required.");
    }

    if (!config.value("registration", json::Json::object()).value("ipv4", json::Json()).is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required.");
    }

    if (!config.value("registration", json::Json::object()).value("port", json::Json()).is_number()) {
        throw std::runtime_error("'registration:port' field is required.");
    }

    if (!config.value("registration", json::Json::object()).value("interval", json::Json()).is_number()) {
        throw std::runtime_error("'registration:interval' field is required.");
    }

    if (!config.value("server", json::Json()).is_object()) {
        throw std::runtime_error("'server' field is required.");
    }

    if (!config.value("server", json::Json::object()).value("port", json::Json()).is_number()) {
        throw std::runtime_error("'server:port' field is required.");
    }

    if (!config.value("managers", json::Json()).is_array()) {
        throw std::runtime_error("'managers' field is required.");
    }

    if (config["managers"].size() == 0) {
        throw std::runtime_error("'managers' array should have at least one entry.");
    }

    if (config.count("managers")) {
        for (const auto& manager: config["managers"]) {
            if (!manager.count("ipv4") || !manager.value("ipv4", json::Json()).is_string()) {
                throw std::runtime_error("Each entry in manager must have ipv4 field.");
            }
            if (!manager.count("username") || !manager.value("username", json::Json()).is_string()) {
                throw std::runtime_error("Each entry in manager must have username field.");
            }
            if (!manager.count("password") || !manager.value("password", json::Json()).is_string()) {
                throw std::runtime_error("Each entry in manager must have password field.");
            }
            if (!manager.count("port") || !manager.value("port", json::Json()).is_number()) {
                throw std::runtime_error("Each entry in manager must have port field.");
            }
            if (!manager.count("slot") || !manager.value("slot", json::Json()).is_number()) {
                throw std::runtime_error("Each entry in manager must have slot field.");
            }
        }
    }
}
}


bool ComputeLoader::load(const json::Json& config) {
    try {
        check_required_fields(config);
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Loading modules configuration failed: " << error.what() << std::endl;
        return false;
    }

    return true;
}
