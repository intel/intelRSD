/*!
 * @brief Definition of Remote Targets Collection POST request validator
 *
 * @copyright Copyright (c) 2017 Intel Corporation
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
 * @header{Files}
 * @file remote_targets_collection.cpp
 */

#include "psme/rest/validators/schemas/remote_targets_collection.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"


using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::AddressesObjectSchema::IscsiObjectSchema::ChapObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::TYPE, VALID_NULLABLE(VALID_ENUM(enums::TargetAuthenticationMethod)),
        constants::RemoteTarget::USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::RemoteTarget::SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::RemoteTarget::MUTUAL_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::RemoteTarget::MUTUAL_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::AddressesObjectSchema::IscsiObjectSchema::TargetLUNObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::LUN, VALID_NUMERIC_TYPED(UINT32),
        constants::RemoteTarget::LOGICAL_DRIVE, VALID_ATTRIBUTE(SimpleObjectSchema),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::AddressesObjectSchema::IscsiObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "iscsi_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::TARGET_LUN, VALID_ARRAY_OF(VALID_ATTRIBUTE(TargetLUNObjectSchema)),
        constants::RemoteTarget::TARGET_IQN, VALID_REGEX(literals::regex::RemoteTarget::TARGET_IQN),
        constants::RemoteTarget::CHAP, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(ChapObjectSchema))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::AddressesObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "address_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::ISCSI, VALID_ATTRIBUTE(IscsiObjectSchema),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::InitiatorObjectSchema::IscsiObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "iscsi_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::INITIATOR_IQN, VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::InitiatorObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "initiator_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::ISCSI, VALID_ATTRIBUTE(IscsiObjectSchema),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& RemoteTargetsCollectionPostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "remote_targets_collection_post",
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::NAME, VALID_OPTIONAL(VALID_JSON_STRING),
        constants::RemoteTarget::TYPE, VALID_OPTIONAL(VALID_JSON_STRING),
        constants::RemoteTarget::ADDRESSES, VALID_ARRAY_OF(VALID_ATTRIBUTE(AddressesObjectSchema)),
        constants::RemoteTarget::INITIATOR, VALID_ARRAY_OF(VALID_ATTRIBUTE(InitiatorObjectSchema)),
        nullptr
    };
    return procedure;
}
