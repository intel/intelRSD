/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "agent-framework/module/constants/regular_expressions.hpp"
#include "psme/rest/validators/schemas/endpoint.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/common.hpp"


using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& EndpointPatchSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EndpointPatchSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::AUTHENTICATION, VALID_OPTIONAL(VALID_ATTRIBUTE(AuthSchema)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EndpointPatchSchema::OemSchema::RackScaleSchema::AuthSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        constants::Endpoint::PASSWORD, VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EndpointPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}
