/*!
 * @brief Definition of Network Device Function endpoint
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
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#NetworkDeviceFunction.NetworkDeviceFunction";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#NetworkDeviceFunction.v1_0_0.NetworkDeviceFunction";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Network Device Function";
    r[Common::DESCRIPTION] = "Network Device Function";
    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;
    r[NetworkDeviceFunction::DEVICE_ENABLED] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS] = json::Json::value_t::null;
    r[Common::LINKS] = json::Json::value_t::object;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::IP_ADDRESS_TYPE] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_IP_ADDRESS] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_NAME] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::INITIATOR_NETMASK] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::TARGET_INFO_VIA_DHCP] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_NAME] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_LUN] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_VLAN_ENABLE] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_VLAN_ID] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::PRIMARY_DNS] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_NAME] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_LUN] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_VLAN_ENABLE] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_VLAN_ID] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::SECONDARY_DNS] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::AUTHENTICATION_METHOD] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::CHAP_USERNAME] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::CHAP_SECRET] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::MUTUAL_CHAP_USERNAME] = json::Json::value_t::null;
    r[NetworkDeviceFunction::ISCSI_BOOT][NetworkDeviceFunction::MUTUAL_CHAP_SECRET] = json::Json::value_t::null;

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


agent_framework::model::attribute::Attributes fill_attributes(const json::Json& json) {
    agent_framework::model::attribute::Attributes attributes{};
    if (json.count(NetworkDeviceFunction::ETHERNET) &&
        json[NetworkDeviceFunction::ETHERNET].count(Common::MAC_ADDRESS)) {
        json::Json ethernet = json::Json::object();
        ethernet[literals::NetworkDeviceFunction::MAC_ADDRESS] = json[NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS];
        attributes.set_value(literals::NetworkDeviceFunction::ETHERNET, ethernet);
    }
    if (json.count(NetworkDeviceFunction::ISCSI_BOOT)) {

        json::Json gami_iscsi_boot = json::Json::object();
        const json::Json& rest_iscsi_boot = json[NetworkDeviceFunction::ISCSI_BOOT];

        auto set_property = [&] (const char* rest_literal, const char* gami_literal) {
            if (rest_iscsi_boot.count(rest_literal)) {
                gami_iscsi_boot[gami_literal] = rest_iscsi_boot[rest_literal];
            }
        };

        set_property(NetworkDeviceFunction::IP_ADDRESS_TYPE, literals::IscsiBoot::IP_ADDRESS_TYPE);
        set_property(NetworkDeviceFunction::INITIATOR_IP_ADDRESS, literals::IscsiBoot::INITIATOR_IP_ADDRESS);
        set_property(NetworkDeviceFunction::INITIATOR_NAME, literals::IscsiBoot::INITIATOR_NAME);
        set_property(NetworkDeviceFunction::INITIATOR_DEFAULT_GATEWAY, literals::IscsiBoot::INITIATOR_DEFAULT_GATEWAY);
        set_property(NetworkDeviceFunction::INITIATOR_NETMASK, literals::IscsiBoot::INITIATOR_NETMASK);
        set_property(NetworkDeviceFunction::TARGET_INFO_VIA_DHCP, literals::IscsiBoot::TARGET_INFO_VIA_DHCP);
        set_property(NetworkDeviceFunction::PRIMARY_TARGET_NAME, literals::IscsiBoot::PRIMARY_TARGET_NAME);
        set_property(NetworkDeviceFunction::PRIMARY_TARGET_IP_ADDRESS, literals::IscsiBoot::PRIMARY_TARGET_IP_ADDRESS);
        set_property(NetworkDeviceFunction::PRIMARY_TARGET_TCP_PORT, literals::IscsiBoot::PRIMARY_TARGET_TCP_PORT);
        set_property(NetworkDeviceFunction::PRIMARY_LUN, literals::IscsiBoot::PRIMARY_LUN);
        set_property(NetworkDeviceFunction::PRIMARY_VLAN_ENABLE, literals::IscsiBoot::PRIMARY_VLAN_ENABLE);
        set_property(NetworkDeviceFunction::PRIMARY_VLAN_ID, literals::IscsiBoot::PRIMARY_VLAN_ID);
        set_property(NetworkDeviceFunction::PRIMARY_DNS, literals::IscsiBoot::PRIMARY_DNS);
        set_property(NetworkDeviceFunction::SECONDARY_TARGET_NAME, literals::IscsiBoot::SECONDARY_TARGET_NAME);
        set_property(NetworkDeviceFunction::SECONDARY_TARGET_IP_ADDRESS, literals::IscsiBoot::SECONDARY_TARGET_IP_ADDRESS);
        set_property(NetworkDeviceFunction::SECONDARY_TARGET_TCP_PORT, literals::IscsiBoot::SECONDARY_TARGET_TCP_PORT);
        set_property(NetworkDeviceFunction::SECONDARY_LUN, literals::IscsiBoot::SECONDARY_LUN);
        set_property(NetworkDeviceFunction::SECONDARY_VLAN_ENABLE, literals::IscsiBoot::SECONDARY_VLAN_ENABLE);
        set_property(NetworkDeviceFunction::SECONDARY_VLAN_ID, literals::IscsiBoot::SECONDARY_VLAN_ID);
        set_property(NetworkDeviceFunction::SECONDARY_DNS, literals::IscsiBoot::SECONDARY_DNS);
        set_property(NetworkDeviceFunction::IP_MASK_DNS_VIA_DHCP, literals::IscsiBoot::IP_MASK_DNS_VIA_DHCP);
        set_property(NetworkDeviceFunction::ROUTER_ADVERTISEMENT_ENABLED, literals::IscsiBoot::ROUTER_ADVERTISEMENT_ENABLED);
        set_property(NetworkDeviceFunction::AUTHENTICATION_METHOD, literals::IscsiBoot::AUTHENTICATION_METHOD);
        set_property(NetworkDeviceFunction::CHAP_USERNAME, literals::IscsiBoot::CHAP_USERNAME);
        set_property(NetworkDeviceFunction::CHAP_SECRET, literals::IscsiBoot::CHAP_SECRET);
        set_property(NetworkDeviceFunction::MUTUAL_CHAP_USERNAME, literals::IscsiBoot::MUTUAL_CHAP_USERNAME);
        set_property(NetworkDeviceFunction::MUTUAL_CHAP_SECRET, literals::IscsiBoot::MUTUAL_CHAP_SECRET);

        attributes.set_value(literals::NetworkDeviceFunction::ISCSI_BOOT, gami_iscsi_boot);
    }
    return attributes;
}
}


endpoint::NetworkDeviceFunction::NetworkDeviceFunction(const std::string& path) : EndpointBase(path) {}


endpoint::NetworkDeviceFunction::~NetworkDeviceFunction() {}


void endpoint::NetworkDeviceFunction::get(const server::Request& req, server::Response& res) {
    auto json = make_prototype();

    auto function = psme::rest::model::find<agent_framework::model::System, agent_framework::model::NetworkDevice, agent_framework::model::NetworkDeviceFunction>(
        req.params).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    json[constants::Common::ID] = req.params[PathParam::NETWORK_DEVICE_FUNCTION_ID];
    endpoint::status_to_json(function, json);

    json[constants::NetworkDeviceFunction::DEVICE_ENABLED] = function.get_device_enabled();
    json[constants::NetworkDeviceFunction::ETHERNET][Common::MAC_ADDRESS] = function.get_mac_address();
    json[Common::LINKS] = json::Json::value_t::object;

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
    static const constexpr char TRANSACTION_NAME[] = "PatchNetworkDeviceFunction";

    const auto& json = JsonValidator::validate_request_body<schema::NetworkDeviceFunctionPatchSchema>(request);
    auto function = psme::rest::model::find<agent_framework::model::System, agent_framework::model::NetworkDevice, agent_framework::model::NetworkDeviceFunction>(
        request.params).get();

    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(function.get_agent_id());
    agent_framework::model::attribute::Attributes attributes = fill_attributes(json);

    auto patch_function = [&, gami_agent] {
        if (!attributes.empty()) {
            const auto& set_component_attributes_request =
                agent_framework::model::requests::SetComponentAttributes{function.get_uuid(), attributes};
            const auto& set_component_attributes_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(
                    set_component_attributes_request);

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

    gami_agent->execute_in_transaction(TRANSACTION_NAME, patch_function);

    get(request, response);
}
