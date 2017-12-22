/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/validators/schemas/endpoints_collection.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"


using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::IdentifierSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::PncDrive::DURABLE_NAME_FORMAT, VALID_JSON_STRING,
        constants::PncDrive::DURABLE_NAME, VALID_JSON_STRING,
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::EntitySchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Endpoint::ENTITY_TYPE, VALID_ENUM(enums::EntityType),
        constants::Endpoint::ENTITY_ROLE, VALID_ENUM(enums::EntityRole),
        constants::Endpoint::ENTITY_LINK, VALID_ATTRIBUTE(SimpleObjectSchema),
        constants::PncDrive::IDENTIFIERS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EndpointsCollectionPostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::NAME, VALID_JSON_STRING,
        constants::Common::DESCRIPTION, VALID_JSON_STRING,
        constants::Endpoint::ENDPOINT_PROTOCOL, VALID_ENUM(enums::StorageProtocol),
        constants::PncDrive::IDENTIFIERS, VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema)),
        constants::Endpoint::CONNECTED_ENTITIES, VALID_ARRAY_OF(VALID_ATTRIBUTE(EntitySchema)),
        nullptr
    };
    return procedure;
}
