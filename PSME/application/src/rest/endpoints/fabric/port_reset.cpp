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
 * */

#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/reset.hpp"
#include "psme/rest/endpoints/fabric/port_reset.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"

#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;


endpoint::PortReset::PortReset(const std::string& path) : EndpointBase(path) {}


endpoint::PortReset::~PortReset() {}


void endpoint::PortReset::post(const server::Request& request, server::Response&) {
    using ResetType = agent_framework::model::enums::ResetType;

    const auto& json = JsonValidator::validate_request_body<schema::ResetPostSchema>(request);

    if (json.count(Common::RESET_TYPE)) {
        const auto port = psme::rest::model::find<agent_framework::model::Fabric, agent_framework::model::Switch, agent_framework::model::Port>(
            request.params).get();

        const auto& allowable_reset_types = port.get_allowed_actions();
        const auto reset_type = json.value(constants::Common::RESET_TYPE, std::string{});
        const auto reset_type_enum = ResetType::from_string(reset_type);

        if (!JsonValidator::validate_allowable_values(allowable_reset_types.get_array(), reset_type_enum)) {
            throw error::ServerException(
                error::ErrorFactory::create_error_for_not_allowable_value(
                    "The Port does not support the requested reset target.",
                    Common::RESET_TYPE, reset_type,
                    endpoint::PathBuilder(constants::Common::ACTIONS)
                        .append(constants::Port::PORT_RESET)
                        .append(Common::ALLOWABLE_RESET_TYPES)
                        .build()
                )
            );
        }

        // Request validation cannot pass, because no ResetTypes are supported by the PNC agent
        // (we do not allow Port reset).
        throw agent_framework::exceptions::InvalidValue("Reset on Fabric Port is not supported!");
    }
}

