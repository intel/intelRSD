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

#include "psme/rest/validators/schemas/rule_collection.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/enum/network.hpp"

using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& RulePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Rule::RULE_ID, VALID_OPTIONAL(VALID_NUMERIC_TYPED(UINT32)),
        constants::Rule::ACTION, VALID_ENUM(enums::AclAction),
        constants::Rule::FORWARD_MIRROR_INTERFACE, VALID_OPTIONAL(VALID_ATTRIBUTE(SimpleObjectSchema)),
        constants::Rule::MIRROR_PORT_REGION, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(SimpleObjectSchema)))),
        constants::Rule::MIRROR_TYPE, VALID_OPTIONAL(VALID_ENUM(enums::AclMirrorType)),
        constants::Rule::CONDITION, VALID_ATTRIBUTE(ConditionSchema),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RulePostSchema::ConditionSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Rule::IP_SOURCE, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(IpSchema))),
        constants::Rule::IP_DESTINATION, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(IpSchema))),
        constants::Rule::MAC_SOURCE, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(MacSchema))),
        constants::Rule::MAC_DESTINATION, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(MacSchema))),
        constants::Rule::VLAN_ID, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(VlanIdSchema))),
        constants::Rule::L4_SOURCE_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(PortSchema))),
        constants::Rule::L4_DESTINATION_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_ATTRIBUTE(PortSchema))),
        constants::Rule::L4_PROTOCOL, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RulePostSchema::ConditionSchema::IpSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Rule::IP_ADDRESS, VALID_JSON_STRING,
        constants::Rule::MASK, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RulePostSchema::ConditionSchema::MacSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::MAC_ADDRESS, VALID_JSON_STRING,
        constants::Rule::MASK, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RulePostSchema::ConditionSchema::VlanIdSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::ID, VALID_NUMERIC_TYPED(UINT32),
        constants::Rule::MASK, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& RulePostSchema::ConditionSchema::PortSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Rule::PORT, VALID_NUMERIC_TYPED(UINT32),
        constants::Rule::MASK, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        nullptr
    };
    return procedure;
}
