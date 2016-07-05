/*!
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
 * */

#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"
#include "psme/rest-ref/endpoints/system.hpp"
#include "psme/rest-ref/endpoints/system_reset.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"

using namespace app::rest;
using namespace app::rest::constants;
using namespace psme::rest;
using namespace psme::rest::utils;



namespace {
json::Value validate_post_request(const server::Request& request) {
    using namespace agent_framework::model::enums;
    using namespace psme::rest::error;
    using json::Pair;

    json::Value supported_values{json::Value::Type::ARRAY};
    for(const auto& value : ResetType::get_values()) {
        supported_values.push_back(value);
    }

    json::Value schema({
        Pair(System::RESET_TYPE,
             JsonValidator::any_of(
                JsonValidator::STRING_TYPE, supported_values, true))
    });

    return  JsonValidator::validate_request_body(request, schema);
}
}

endpoint::SystemReset::SystemReset(const std::string& path) : Endpoint(path) {}
endpoint::SystemReset::~SystemReset() {}


void endpoint::SystemReset::post(const server::Request& request, server::Response& response) {

    std::string agent_id{};
    std::string system_uuid{};
    std::string parent_uuid{};

    // Gets necessary data from model
    {
        auto system = model::Find<agent_framework::model::System>(
                                        request.params[PathParam::SYSTEM_ID]).get_one();

        agent_id =  system->get_agent_id();
        system_uuid = system->get_uuid();
        parent_uuid = system->get_parent_uuid();
    }

    const auto& json = validate_post_request(request);
    const auto& reset_type = json[constants::System::RESET_TYPE].as_string();
    agent_framework::model::attribute::Attributes attributes{};
    attributes.set_value(
        agent_framework::model::literals::System::POWER_STATE, reset_type);

    agent_framework::model::requests::SetComponentAttributes
    set_component_attributes_request{system_uuid, attributes};

    // Call set component attribute method
    const auto& manager = psme::core::agent::AgentManager::get_instance();
    manager->call_method
        <agent_framework::model::responses::SetComponentAttributes>(
            agent_id, set_component_attributes_request);

    psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
        agent_framework::model::enums::Component::System)->
        load(manager->get_agent(agent_id), parent_uuid,
             system_uuid, false);

    response.set_status(server::status_2XX::NO_CONTENT);
}
