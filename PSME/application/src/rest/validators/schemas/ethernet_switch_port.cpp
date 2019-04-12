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

#include "psme/rest/validators/schemas/ethernet_switch_port.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/enum/network.hpp"

using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& EthernetSwitchPortPatchSchema::LinksSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitchPort::PORT_MEMBERS, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(SimpleObjectSchema)))),
        constants::EthernetSwitchPort::PRIMARY_VLAN, VALID_OPTIONAL(VALID_ATTRIBUTE(SimpleObjectSchema)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPortPatchSchema::PriorityFlowControlSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitchPort::ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::EthernetSwitchPort::ENABLED_PRIORITIES, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_NUMERIC_RANGE(UINT32, 0 , 7), 0, 8))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPortPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitchPort::ADMINISTRATIVE_STATE, VALID_OPTIONAL(VALID_ENUM(enums::AdministrativeState)),
        // per Metadata, these values have signed int64 type, so this is the type that PATCH command has to accept
        constants::EthernetSwitchPort::LINK_SPEED, VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT64)),
        constants::EthernetSwitchPort::FRAME_SIZE, VALID_OPTIONAL(VALID_NUMERIC_TYPED(INT64)),
        constants::EthernetSwitchPort::AUTOSENSE, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        constants::EthernetSwitchPort::LLDP_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        constants::EthernetSwitchPort::DCBX_STATE, VALID_OPTIONAL(VALID_ENUM(enums::DcbxState)),
        constants::EthernetSwitchPort::PRIORITY_FLOW_CONTROL, VALID_OPTIONAL(VALID_ATTRIBUTE(PriorityFlowControlSchema)),
        constants::Common::LINKS, VALID_OPTIONAL(VALID_ATTRIBUTE(LinksSchema)),
        nullptr
    };
    return procedure;
}
