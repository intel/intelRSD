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

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/endpoints/ethernet/ethernet_switch.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/ethernet_switch.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"
#include "agent-framework/module/responses/common.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::error;
using namespace psme::rest::validators;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitch.EthernetSwitch";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EthernetSwitch.v1_1_0.EthernetSwitch";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Ethernet Switch";
    r[Common::DESCRIPTION] = "Ethernet Switch description";
    r[EthernetSwitch::SWITCH_ID] = json::Json::value_t::null;
    r[Common::MANUFACTURER] = json::Json::value_t::null;
    r[Common::MODEL] = json::Json::value_t::null;
    r[EthernetSwitch::MANUFACTURING_DATE] = json::Json::value_t::null;
    r[Common::SERIAL_NUMBER] = json::Json::value_t::null;
    r[Common::PART_NUMBER] = json::Json::value_t::null;
    r[EthernetSwitch::FIRMWARE_NAME] = json::Json::value_t::null;
    r[Common::FIRMWARE_VERSION] = json::Json::value_t::null;
    r[EthernetSwitch::ROLE] = json::Json::value_t::null;
    r[EthernetSwitch::MAX_ACL_NUMBER] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[Common::OEM] = json::Json::value_t::object;

    r[EthernetSwitch::PORTS] = json::Json::value_t::object;
    r[EthernetSwitch::ACLS] = json::Json::value_t::object;
    r[Common::LINKS][Common::CHASSIS] = json::Json::value_t::null;
    r[Common::LINKS][Common::MANAGED_BY] = json::Json::value_t::array;
    r[Common::LINKS][Common::OEM] = json::Json::value_t::object;

    r[EthernetSwitch::PFC_ENABLED] = json::Json::value_t::null;
    r[EthernetSwitch::LLDP_ENABLED] = json::Json::value_t::null;
    r[EthernetSwitch::ETS_ENABLED] = json::Json::value_t::null;
    r[EthernetSwitch::DCBX_ENABLED] = json::Json::value_t::null;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::APPLICATION_PROTOCOL] = json::Json::value_t::array;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING] = json::Json::value_t::array;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::BANDWIDTH_ALLOCATION] = json::Json::value_t::array;

    return r;
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::EthernetSwitch::PFC_ENABLED,  constants::EthernetSwitch::PFC_ENABLED},
    {agent_framework::model::literals::EthernetSwitch::LLDP_ENABLED, constants::EthernetSwitch::LLDP_ENABLED},
    {agent_framework::model::literals::EthernetSwitch::ETS_ENABLED,  constants::EthernetSwitch::ETS_ENABLED},
    {agent_framework::model::literals::EthernetSwitch::DCBX_ENABLED, constants::EthernetSwitch::DCBX_ENABLED},
    {agent_framework::model::literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL,
        endpoint::PathBuilder(constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION)
            .append(constants::EthernetSwitch::APPLICATION_PROTOCOL).build()},
    {agent_framework::model::literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING,
        endpoint::PathBuilder(constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION)
            .append(constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING).build()},
    {agent_framework::model::literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION,
        endpoint::PathBuilder(constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION)
            .append(constants::EthernetSwitch::BANDWIDTH_ALLOCATION).build()},
};


agent_framework::model::attribute::Attributes fill_attributes(json::Json& json) {
    agent_framework::model::attribute::Attributes attributes{};

    if (json.count(constants::EthernetSwitch::PFC_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::PFC_ENABLED,
                             json.value(constants::EthernetSwitch::PFC_ENABLED, bool{}));
    }
    if (json.count(constants::EthernetSwitch::LLDP_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::LLDP_ENABLED,
                             json.value(constants::EthernetSwitch::LLDP_ENABLED, bool{}));
    }
    if (json.count(constants::EthernetSwitch::ETS_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::ETS_ENABLED,
                             json.value(constants::EthernetSwitch::ETS_ENABLED, bool{}));
    }
    if (json.count(constants::EthernetSwitch::DCBX_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::DCBX_ENABLED,
                             json.value(constants::EthernetSwitch::DCBX_ENABLED, bool{}));
    }
    if (json.count(constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION)) {
        const auto& dcbx_config = json[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION];

        if (dcbx_config.count(constants::EthernetSwitch::APPLICATION_PROTOCOL)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosApplicationProtocol> qap_array{};

            if (!dcbx_config.value(constants::EthernetSwitch::APPLICATION_PROTOCOL, json::Json()).is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::APPLICATION_PROTOCOL]) {
                    qap_array.add_entry({agent_framework::model::enums::TransportLayerProtocol::
                                         from_string(array_elem.value(constants::EthernetSwitch::PROTOCOL, std::string{})),
                                         array_elem.value(constants::EthernetSwitch::PORT, std::uint16_t{}),
                                         array_elem.value(constants::EthernetSwitch::PRIORITY, std::uint16_t{})});
                }
            }

            attributes.set_value(agent_framework::model::literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL, qap_array.to_json());
        }

        if (dcbx_config.count(constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosPriorityGroupMapping> qpm_array{};

            if (!dcbx_config.value(constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING, json::Json()).is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING]) {
                    qpm_array.add_entry({array_elem.value(constants::EthernetSwitch::PRIORITY_GROUP, std::uint16_t{}),
                                         array_elem.value(constants::EthernetSwitch::PRIORITY, std::uint16_t{})});
                }
            }

            attributes.set_value(agent_framework::model::literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING, qpm_array.to_json());
        }

        if (dcbx_config.count(constants::EthernetSwitch::BANDWIDTH_ALLOCATION)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosBandwidthAllocation> qba_array{};

            if (!dcbx_config.value(constants::EthernetSwitch::BANDWIDTH_ALLOCATION, json::Json()).is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::BANDWIDTH_ALLOCATION]) {
                    qba_array.add_entry({array_elem.value(constants::EthernetSwitch::PRIORITY_GROUP, std::uint16_t{}),
                                         array_elem.value(constants::EthernetSwitch::BANDWIDTH_PERCENT, std::uint16_t{})});
                }
            }

            attributes.set_value(agent_framework::model::literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION, qba_array.to_json());
        }
    }

    return attributes;
}

}


endpoint::EthernetSwitch::EthernetSwitch(const std::string& path) : EndpointBase(path) {}


endpoint::EthernetSwitch::~EthernetSwitch() {}


void endpoint::EthernetSwitch::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();
    auto s = model::find<agent_framework::model::EthernetSwitch>(request.params).get();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::ETHERNET_SWITCH_ID];
    r[Common::NAME] = constants::EthernetSwitch::SWITCH + request.params[PathParam::ETHERNET_SWITCH_ID];
    r[constants::EthernetSwitch::SWITCH_ID] = s.get_switch_identifier();

    const auto& fru = s.get_fru_info();
    r[Common::MANUFACTURER] = fru.get_manufacturer();
    r[Common::MODEL] = fru.get_model_number();
    r[constants::EthernetSwitch::MANUFACTURING_DATE] = s.get_manufacturing_date();
    r[Common::SERIAL_NUMBER] = fru.get_serial_number();
    r[Common::PART_NUMBER] = fru.get_part_number();
    r[constants::EthernetSwitch::FIRMWARE_NAME] = s.get_firmware_name();
    r[Common::FIRMWARE_VERSION] = s.get_firmware_version();
    r[constants::EthernetSwitch::ROLE] = s.get_role();
    r[constants::EthernetSwitch::MAX_ACL_NUMBER] = s.get_max_acl_number();

    psme::rest::endpoint::status_to_json(s, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = endpoint::HealthRollup<agent_framework::model::EthernetSwitch>()
        .get(s.get_uuid());
    r[constants::EthernetSwitch::PORTS][Common::ODATA_ID] = PathBuilder(request)
        .append(constants::EthernetSwitch::PORTS)
        .build();
    r[constants::EthernetSwitch::ACLS][Common::ODATA_ID] = PathBuilder(request)
        .append(constants::EthernetSwitch::ACLS)
        .build();

    r[Common::LINKS][constants::Common::CHASSIS][Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Chassis, s.get_chassis());

    json::Json manager = json::Json();
    manager[Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Manager, s.get_parent_uuid());

    r[Common::LINKS][Common::MANAGED_BY].push_back(std::move(manager));

    r[constants::EthernetSwitch::PFC_ENABLED] = s.get_pfc_enabled();
    r[constants::EthernetSwitch::LLDP_ENABLED] = s.get_lldp_enabled();
    r[constants::EthernetSwitch::ETS_ENABLED] = s.get_ets_enabled();
    r[constants::EthernetSwitch::DCBX_ENABLED] = s.get_dcbx_enabled();

    for (const auto& application_protocol : s.get_qos_application_protocol()) {
        json::Json array_elem(json::Json::value_t::object);
        array_elem[constants::EthernetSwitch::PRIORITY] = application_protocol.get_priority();
        array_elem[constants::EthernetSwitch::PROTOCOL] = application_protocol.get_protocol();
        array_elem[constants::EthernetSwitch::PORT] = application_protocol.get_port();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::APPLICATION_PROTOCOL].
            push_back(std::move(array_elem));
    }

    for (const auto& priority_mapping : s.get_qos_priority_group_mapping()) {
        json::Json array_elem(json::Json::value_t::object);
        array_elem[constants::EthernetSwitch::PRIORITY_GROUP] = priority_mapping.get_priority_group();
        array_elem[constants::EthernetSwitch::PRIORITY] = priority_mapping.get_priority();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING].
            push_back(std::move(array_elem));
    }

    for (const auto& bandwidth_mapping : s.get_qos_bandwidth_allocation()) {
        json::Json array_elem(json::Json::value_t::object);
        array_elem[constants::EthernetSwitch::PRIORITY_GROUP] = bandwidth_mapping.get_priority_group();
        array_elem[constants::EthernetSwitch::BANDWIDTH_PERCENT] = bandwidth_mapping.get_bandwidth_percent();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::BANDWIDTH_ALLOCATION].
            push_back(std::move(array_elem));
    }

    set_response(response, r);
}

void endpoint::EthernetSwitch::patch(const server::Request& request, server::Response& response) {
    using namespace agent_framework::model;
    static const constexpr char TRANSACTION_NAME[] = "PatchEthernetSwitch";

    auto json = JsonValidator::validate_request_body<schema::EthernetSwitchPatchSchema>(request);
    auto attributes = fill_attributes(json);

    if (!attributes.empty()) {
        auto ethernet_switch = psme::rest::model::find<agent_framework::model::EthernetSwitch>(
            request.params).get();

        const auto& set_component_attributes_request =
            requests::SetComponentAttributes{ethernet_switch.get_uuid(), attributes};

        auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(ethernet_switch.get_agent_id());

        auto patch_ethernet_switch = [&, gami_agent] {
            const auto& set_component_attributes_response =
                gami_agent->execute<responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitch)->
                load(gami_agent,
                     ethernet_switch.get_parent_uuid(),
                     agent_framework::model::enums::Component::Manager,
                     ethernet_switch.get_uuid(),
                     false);
        };

        gami_agent->execute_in_transaction(TRANSACTION_NAME, patch_ethernet_switch);
    }
    get(request, response);
}
