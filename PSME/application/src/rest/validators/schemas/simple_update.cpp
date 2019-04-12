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

#include "agent-framework/module/enum/common.hpp"
#include "psme/rest/validators/schemas/simple_update.hpp"
#include "psme/rest/constants/constants.hpp"

#include <limits>



using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& SimpleUpdatePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::UpdateService::IMAGE_URI, VALID_JSON_STRING,
        constants::UpdateService::TRANSFER_PROTOCOL, VALID_OPTIONAL(VALID_ENUM(enums::TransferProtocol)),
        constants::UpdateService::TARGETS, VALID_ARRAY_SIZE_OF(VALID_JSON_STRING, 1, std::numeric_limits<int>::max()),
        nullptr
    };
    return procedure;
}


