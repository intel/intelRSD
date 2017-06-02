/*!
 * @brief Definition of Network Device Function endpoint
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
 * @file network_device_function.cpp
 */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/system/network_device_function.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/network_device_function.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace agent_framework::module;
namespace literals = agent_framework::model::literals;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/NetworkInterfaces/__NETWORK_INTERFACE_ID__/NetworkDeviceFunctions/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#NetworkDeviceFunction.v1_0_0.NetworkDeviceFunction";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Network Device Function";
    r[Common::DESCRIPTION] = "Network Device Function";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::DEVICE_ENABLED] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS] = json::Value::Type::NIL;
    r[Common::LINKS] = json::Value::Type::OBJECT;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::IP_ADDRESS_TYPE] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_IP_ADDRESS] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_NAME] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_NETMASK] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::TARGET_INFO_VIA_DHCP] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_NAME] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_LUN] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_VLAN_ENABLE] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_VLAN_ID] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_DNS] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_NAME] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_LUN] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_VLAN_ENABLE] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_VLAN_ID] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_DNS] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::AUTHENTICATION_METHOD] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::CHAP_USERNAME] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::CHAP_SECRET] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::MUTUAL_CHAP_USERNAME] = json::Value::Type::NIL;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::MUTUAL_CHAP_SECRET] = json::Value::Type::NIL;

    return r;
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ETHERNET).append(literals::NetworkDeviceFunction::MAC_ADDRESS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ETHERNET).append(Common::MAC_ADDRESS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::INITIATOR_IP_ADDRESS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::INITIATOR_IP_ADDRESS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::INITIATOR_NAME).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::INITIATOR_NAME).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::INITIATOR_NETMASK).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::INITIATOR_NETMASK).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::TARGET_INFO_VIA_DHCP).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::TARGET_INFO_VIA_DHCP).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_TARGET_NAME).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_TARGET_NAME).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_LUN).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_LUN).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_VLAN_ENABLE).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_VLAN_ENABLE).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_VLAN_ID).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_VLAN_ID).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::PRIMARY_DNS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::PRIMARY_DNS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_TARGET_NAME).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_TARGET_NAME).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_LUN).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_LUN).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_VLAN_ENABLE).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_VLAN_ENABLE).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_VLAN_ID).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_VLAN_ID).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::SECONDARY_DNS).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::SECONDARY_DNS).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::AUTHENTICATION_METHOD).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::AUTHENTICATION_METHOD).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::CHAP_USERNAME).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::CHAP_USERNAME).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::CHAP_SECRET).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::CHAP_SECRET).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::MUTUAL_CHAP_USERNAME).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::MUTUAL_CHAP_USERNAME).build()},
    {endpoint::PathBuilder(literals::NetworkDeviceFunction::ISCSI_BOOT).append(literals::IscsiBoot::MUTUAL_CHAP_SECRET).build(),
        endpoint::PathBuilder(NetworkDeviceFunction::ISCSI_BOOT).append(NetworkDeviceFunction::MUTUAL_CHAP_SECRET).build()}
};

agent_framework::model::attribute::Attributes fill_attributes(const json::Value& json) {
    agent_framework::model::attribute::Attributes attributes{};
    if (json.is_member(NetworkDeviceFunction::ETHERNET) &&
        json[NetworkDeviceFunction::ETHERNET].is_member(Common::MAC_ADDRESS)) {
        Json::Value ethernet{Json::ValueType::objectValue};
        if (json[NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS].is_null()) {
            ethernet[literals::NetworkDeviceFunction::MAC_ADDRESS] = Json::Value::null;
        } else {
            ethernet[literals::NetworkDeviceFunction::MAC_ADDRESS] = json[NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS].as_string();
        }
        attributes.set_value(literals::NetworkDeviceFunction::ETHERNET, ethernet);
    }
    if (json.is_member(NetworkDeviceFunction::ISCSI_BOOT)) {
        Json::Value gami_iscsi_boot{Json::ValueType::objectValue};
        const json::Value& rest_iscsi_boot = json[NetworkDeviceFunction::ISCSI_BOOT];

        auto has_value_to_set = [&] (const char* rest_literal, const char* gami_literal) {
            if (rest_iscsi_boot.is_member(rest_literal)) {
                if (rest_iscsi_boot[rest_literal].is_null()) {
                    gami_iscsi_boot[gami_literal] = Json::Value::null;
                } else {
                    return true;
                }
            }
            return false;
        };
        auto set_string = [&] (const char* rest_literal, const char* gami_literal) {
            if (has_value_to_set(rest_literal, gami_literal)) {
                gami_iscsi_boot[gami_literal] = rest_iscsi_boot[rest_literal].as_string();
            }
        };
        auto set_boolean = [&] (const char* rest_literal, const char* gami_literal) {
            if (has_value_to_set(rest_literal, gami_literal)) {
                gami_iscsi_boot[gami_literal] = rest_iscsi_boot[rest_literal].as_bool();
            }
        };
        auto set_uint32 = [&] (const char* rest_literal, const char* gami_literal) {
            if (has_value_to_set(rest_literal, gami_literal)) {
                gami_iscsi_boot[gami_literal] = rest_iscsi_boot[rest_literal].as_uint();
            }
        };
        set_string(NetworkDeviceFunction::IP_ADDRESS_TYPE, literals::IscsiBoot::IP_ADDRESS_TYPE);
        set_string(NetworkDeviceFunction::INITIATOR_IP_ADDRESS, literals::IscsiBoot::INITIATOR_IP_ADDRESS);
        set_string(NetworkDeviceFunction::INITIATOR_NAME, literals::IscsiBoot::INITIATOR_NAME);
        set_string(NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY, literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY);
        set_string(NetworkDeviceFunction::INITIATOR_NETMASK, literals::IscsiBoot::INITIATOR_NETMASK);
        set_boolean(NetworkDeviceFunction::TARGET_INFO_VIA_DHCP, literals::IscsiBoot::TARGET_INFO_VIA_DHCP);
        set_string(NetworkDeviceFunction::PRIMARY_TARGET_NAME, literals::IscsiBoot::PRIMARY_TARGET_NAME);
        set_string(NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS, literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS);
        set_uint32(NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT, literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT);
        set_uint32(NetworkDeviceFunction::PRIMARY_LUN, literals::IscsiBoot::PRIMARY_LUN);
        set_boolean(NetworkDeviceFunction::PRIMARY_VLAN_ENABLE, literals::IscsiBoot::PRIMARY_VLAN_ENABLE);
        set_uint32(NetworkDeviceFunction::PRIMARY_VLAN_ID, literals::IscsiBoot::PRIMARY_VLAN_ID);
        set_string(NetworkDeviceFunction::PRIMARY_DNS, literals::IscsiBoot::PRIMARY_DNS);
        set_string(NetworkDeviceFunction::SECONDARY_TARGET_NAME, literals::IscsiBoot::SECONDARY_TARGET_NAME);
        set_string(NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS, literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS);
        set_uint32(NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT, literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT);
        set_uint32(NetworkDeviceFunction::SECONDARY_LUN, literals::IscsiBoot::SECONDARY_LUN);
        set_boolean(NetworkDeviceFunction::SECONDARY_VLAN_ENABLE, literals::IscsiBoot::SECONDARY_VLAN_ENABLE);
        set_uint32(NetworkDeviceFunction::SECONDARY_VLAN_ID, literals::IscsiBoot::SECONDARY_VLAN_ID);
        set_string(NetworkDeviceFunction::SECONDARY_DNS, literals::IscsiBoot::SECONDARY_DNS);
        set_boolean(NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP, literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP);
        set_boolean(NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED, literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED);
        set_string(NetworkDeviceFunction::AUTHENTICATION_METHOD, literals::IscsiBoot::AUTHENTICATION_METHOD);
        set_string(NetworkDeviceFunction::CHAP_USERNAME, literals::IscsiBoot::CHAP_USERNAME);
        set_string(NetworkDeviceFunction::CHAP_SECRET, literals::IscsiBoot::CHAP_SECRET);
        set_string(NetworkDeviceFunction::MUTUAL_CHAP_USERNAME, literals::IscsiBoot::MUTUAL_CHAP_USERNAME);
        set_string(NetworkDeviceFunction::MUTUAL_CHAP_SECRET, literals::IscsiBoot::MUTUAL_CHAP_SECRET);

        attributes.set_value(literals::NetworkDeviceFunction::ISCSI_BOOT, gami_iscsi_boot);
    }
    return attributes;
}
}

endpoint::NetworkDeviceFunction::NetworkDeviceFunction(const std::string& path) : EndpointBase(path) {}
endpoint::NetworkDeviceFunction::~NetworkDeviceFunction() {}

void endpoint::NetworkDeviceFunction::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    auto system_id = psme::rest::model::Find<agent_framework::model::System>(
            req.params[PathParam::SYSTEM_ID]).get_one()->get_id();
    auto network_interface_id = psme::rest::model::Find<agent_framework::model::NetworkDevice>(
            req.params[PathParam::NETWORK_INTERFACE_ID]).
            via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get_one()->get_id();
    auto function = psme::rest::model::Find<agent_framework::model::NetworkDeviceFunction>(
            req.params[PathParam::NETWORK_DEVICE_FUNCTION_ID]).
            via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).
            via<agent_framework::model::NetworkDevice>(req.params[PathParam::NETWORK_INTERFACE_ID]).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                     std::regex("__SYSTEM_ID__"),
                                                     std::to_string(system_id));
    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                     std::regex("__NETWORK_INTERFACE_ID__"),
                                                     std::to_string(network_interface_id));

    json[constants::Common::ID] = req.params[PathParam::NETWORK_DEVICE_FUNCTION_ID];
    endpoint::status_to_json(function,json);

    json[constants::NetworkDeviceFunction::DEVICE_ENABLED] = function.get_device_enabled();
    json[constants::NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS] = function.get_mac_address();
    json[Common::LINKS] = json::Value::Type::OBJECT;

    const auto& iscsi_boot = function.get_iscsi_boot();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::IP_ADDRESS_TYPE] =
            iscsi_boot.get_ip_address_type().to_string();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::INITIATOR_IP_ADDRESS] =
            iscsi_boot.get_initiator_address();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::INITIATOR_NAME] =
            iscsi_boot.get_initiator_name();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY] =
            iscsi_boot.get_initiator_default_gateway();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::INITIATOR_NETMASK] =
            iscsi_boot.get_initiator_netmask();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::TARGET_INFO_VIA_DHCP] =
            iscsi_boot.get_target_info_via_dhcp();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_TARGET_NAME] =
            iscsi_boot.get_primary_target_name();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS] =
            iscsi_boot.get_primary_target_address();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT] =
            iscsi_boot.get_primary_target_port();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_LUN] =
            iscsi_boot.get_primary_lun();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_VLAN_ENABLE] =
            iscsi_boot.get_primary_vlan_enable();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_VLAN_ID] =
            iscsi_boot.get_primary_vlan_id();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::PRIMARY_DNS] =
            iscsi_boot.get_primary_dns();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_TARGET_NAME] =
            iscsi_boot.get_secondary_target_name();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS] =
            iscsi_boot.get_secondary_target_address();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT] =
            iscsi_boot.get_secondary_target_port();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_LUN] =
            iscsi_boot.get_secondary_lun();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_VLAN_ENABLE] =
            iscsi_boot.get_secondary_vlan_enable();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_VLAN_ID] =
            iscsi_boot.get_secondary_vlan_id();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::SECONDARY_DNS] =
            iscsi_boot.get_secondary_dns();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP] =
            iscsi_boot.get_ip_mask_dns_via_dhcp();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED] =
            iscsi_boot.get_router_advertisement_enabled();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::AUTHENTICATION_METHOD] =
            iscsi_boot.get_authentication_method().to_string();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::CHAP_USERNAME] =
            iscsi_boot.get_chap_username();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::CHAP_SECRET] =
            iscsi_boot.get_chap_secret();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::MUTUAL_CHAP_USERNAME] =
            iscsi_boot.get_mutual_chap_username();
    json[constants::NetworkDeviceFunction::ISCSI_BOOT][constants::NetworkDeviceFunction::MUTUAL_CHAP_SECRET] =
            iscsi_boot.get_mutual_chap_secret();

    set_response(res, json);
}

void endpoint::NetworkDeviceFunction::patch(const server::Request& request, server::Response& response) {
    const auto& json = JsonValidator::validate_request_body<schema::NetworkDeviceFunctionPatchSchema>(request);
    auto function = psme::rest::model::Find<agent_framework::model::NetworkDeviceFunction>(
        request.params[PathParam::NETWORK_DEVICE_FUNCTION_ID]).
        via<agent_framework::model::System>(request.params[PathParam::SYSTEM_ID]).
        via<agent_framework::model::NetworkDevice>(request.params[PathParam::NETWORK_INTERFACE_ID]).get();

    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(function.get_agent_id());
    agent_framework::model::attribute::Attributes attributes = fill_attributes(json);

    auto patch_function = [&, gami_agent] {
        if (!attributes.empty()) {
            const auto& set_component_attributes_request =
                agent_framework::model::requests::SetComponentAttributes{function.get_uuid(), attributes};
            const auto& set_component_attributes_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = psme::rest::error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw psme::rest::error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::NetworkDeviceFunction)->
                load(gami_agent,
                     function.get_parent_uuid(),
                     agent_framework::model::enums::Component::NetworkDevice,
                     function.get_uuid(),
                     false);
        }
    };

    gami_agent->execute_in_transaction(patch_function);

    get(request, response);
}
