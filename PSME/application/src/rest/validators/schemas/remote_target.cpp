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

#include "psme/rest/validators/schemas/remote_target.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/storage.hpp"


using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "remote_target_patch",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::ADDRESSES, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(AddressObjectSchema))),
        constants::RemoteTarget::INITIATOR, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(InitiatorObjectSchema))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::AddressObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            "address_object",
            jsonrpc::PARAMS_BY_NAME,
            constants::RemoteTarget::ISCSI, VALID_ATTRIBUTE(IscsiObjectSchema),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::AddressObjectSchema::IscsiObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            "iscsi_object",
            jsonrpc::PARAMS_BY_NAME,
            constants::RemoteTarget::CHAP, VALID_NULLABLE(VALID_ATTRIBUTE(ChapObjectSchema)),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::AddressObjectSchema::IscsiObjectSchema::ChapObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            "chap_object",
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
const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::InitiatorObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "initiator_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::ISCSI, VALID_ATTRIBUTE(IscsiObjectSchema),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RemoteTargetPatchSchema::InitiatorObjectSchema::IscsiObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        "iscsi_object",
        jsonrpc::PARAMS_BY_NAME,
        constants::RemoteTarget::INITIATOR_IQN, VALID_NULLABLE(VALID_JSON_STRING),
        nullptr
    };
    return procedure;
}
