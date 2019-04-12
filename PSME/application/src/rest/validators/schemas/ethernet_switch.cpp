/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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

#include "psme/rest/validators/schemas/ethernet_switch.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/network.hpp"

using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& EthernetSwitchPatchSchema::QosApplicationProtocolSchema::get_procedure() {
    static constexpr const uint32_t MAX_PRIORITY{7};
    static constexpr const uint32_t MAX_PORT{65535};
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitch::PRIORITY, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY)),
        constants::EthernetSwitch::PORT, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PORT)),
        constants::EthernetSwitch::PROTOCOL, VALID_OPTIONAL(VALID_ENUM(enums::TransportLayerProtocol)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPatchSchema::QosPriorityMappingSchema::get_procedure() {
    static constexpr const uint32_t MAX_PRIORITY{7};
    static constexpr const uint32_t MAX_PRIORITY_GROUP{15};
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitch::PRIORITY, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY)),
        constants::EthernetSwitch::PRIORITY_GROUP, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY_GROUP)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPatchSchema::QosBandwidthAllocationSchema::get_procedure() {
    static constexpr const uint32_t MAX_PRIORITY_GROUP{15};
    static constexpr const uint32_t MAX_PERCENT{100};
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitch::PRIORITY_GROUP, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PRIORITY_GROUP)),
        constants::EthernetSwitch::BANDWIDTH_PERCENT, VALID_OPTIONAL(VALID_NUMERIC_EQLT(UINT32, MAX_PERCENT)),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPatchSchema::DcbxSharedConfigurationSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitch::APPLICATION_PROTOCOL, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosApplicationProtocolSchema)))),
        constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosPriorityMappingSchema)))),
        constants::EthernetSwitch::BANDWIDTH_ALLOCATION, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ATTRIBUTE(QosBandwidthAllocationSchema)))),
        nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& EthernetSwitchPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::EthernetSwitch::PFC_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::EthernetSwitch::LLDP_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::EthernetSwitch::ETS_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::EthernetSwitch::DCBX_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION, VALID_OPTIONAL(VALID_ATTRIBUTE(DcbxSharedConfigurationSchema)),
        nullptr
    };
    return procedure;
}
