/*!
 * @brief Definition of Network Device Function PATCH request validator
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file network_device_function.cpp
 */

#include "psme/rest/validators/schemas/network_device_function.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/compute.hpp"


using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;



const jsonrpc::ProcedureValidator& NetworkDeviceFunctionPatchSchema::EthernetObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::MAC_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& NetworkDeviceFunctionPatchSchema::IscsiBootObjectSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::NetworkDeviceFunction::IP_ADDRESS_TYPE, VALID_OPTIONAL(VALID_NULLABLE(VALID_ENUM(enums::IPAddressType))),
        constants::NetworkDeviceFunction::INITIATOR_IP_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::INITIATOR_NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::INITIATOR_NETMASK, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::TARGET_INFO_VIA_DHCP, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::NetworkDeviceFunction::PRIMARY_TARGET_NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        constants::NetworkDeviceFunction::PRIMARY_LUN, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQGT(INT64,0))),
        constants::NetworkDeviceFunction::PRIMARY_VLAN_ENABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::NetworkDeviceFunction::PRIMARY_VLAN_ID, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        constants::NetworkDeviceFunction::PRIMARY_DNS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::SECONDARY_TARGET_NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        constants::NetworkDeviceFunction::SECONDARY_LUN, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_EQGT(INT64,0))),
        constants::NetworkDeviceFunction::SECONDARY_VLAN_ENABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::NetworkDeviceFunction::SECONDARY_VLAN_ID, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT32))),
        constants::NetworkDeviceFunction::SECONDARY_DNS, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        constants::NetworkDeviceFunction::AUTHENTICATION_METHOD, VALID_OPTIONAL(VALID_NULLABLE(VALID_ENUM(enums::FunctionAuthenticationMethod))),
        constants::NetworkDeviceFunction::CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::MUTUAL_CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        constants::NetworkDeviceFunction::MUTUAL_CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& NetworkDeviceFunctionPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::NetworkDeviceFunction::ETHERNET, VALID_OPTIONAL(VALID_ATTRIBUTE(EthernetObjectSchema)),
        constants::NetworkDeviceFunction::ISCSI_BOOT, VALID_OPTIONAL(VALID_ATTRIBUTE(IscsiBootObjectSchema)),
        // TODO:validate IP addresses and MAC addresses
        nullptr
    };
    return procedure;
}
