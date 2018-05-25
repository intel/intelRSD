/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitch.EthernetSwitch";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitch.v1_1_0.EthernetSwitch";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Ethernet Switch";
    r[Common::DESCRIPTION] = "Ethernet Switch description";
    r[EthernetSwitch::SWITCH_ID] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[EthernetSwitch::MANUFACTURING_DATE] = json::Value::Type::NIL;
    r[Common::SERIAL_NUMBER] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[EthernetSwitch::FIRMWARE_NAME] = json::Value::Type::NIL;
    r[Common::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[EthernetSwitch::ROLE] = json::Value::Type::NIL;
    r[EthernetSwitch::MAX_ACL_NUMBER] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::OEM] = json::Value::Type::OBJECT;

    r[EthernetSwitch::PORTS] = json::Value::Type::OBJECT;
    r[EthernetSwitch::ACLS] = json::Value::Type::OBJECT;
    r[Common::LINKS][Common::CHASSIS] = json::Value::Type::NIL;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    r[EthernetSwitch::PFC_ENABLED] = json::Value::Type::NIL;
    r[EthernetSwitch::LLDP_ENABLED] = json::Value::Type::NIL;
    r[EthernetSwitch::ETS_ENABLED] = json::Value::Type::NIL;
    r[EthernetSwitch::DCBX_ENABLED] = json::Value::Type::NIL;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::APPLICATION_PROTOCOL] = json::Value::Type::ARRAY;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING] = json::Value::Type::ARRAY;
    r[EthernetSwitch::DCBX_SHARED_CONFIGURATION][EthernetSwitch::BANDWIDTH_ALLOCATION] = json::Value::Type::ARRAY;

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


agent_framework::model::attribute::Attributes fill_attributes(json::Value& json) {
    agent_framework::model::attribute::Attributes attributes{};

    if (json.is_member(constants::EthernetSwitch::PFC_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::PFC_ENABLED,
                             json[constants::EthernetSwitch::PFC_ENABLED].as_bool());
    }
    if (json.is_member(constants::EthernetSwitch::LLDP_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::LLDP_ENABLED,
                             json[constants::EthernetSwitch::LLDP_ENABLED].as_bool());
    }
    if (json.is_member(constants::EthernetSwitch::ETS_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::ETS_ENABLED,
                             json[constants::EthernetSwitch::ETS_ENABLED].as_bool());
    }
    if (json.is_member(constants::EthernetSwitch::DCBX_ENABLED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitch::DCBX_ENABLED,
                             json[constants::EthernetSwitch::DCBX_ENABLED].as_bool());
    }
    if (json.is_member(constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION)) {
        const auto& dcbx_config = json[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION];

        if (dcbx_config.is_member(constants::EthernetSwitch::APPLICATION_PROTOCOL)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosApplicationProtocol> qap_array{};

            if (!dcbx_config[constants::EthernetSwitch::APPLICATION_PROTOCOL].is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::APPLICATION_PROTOCOL]) {
                    qap_array.add_entry({agent_framework::model::enums::TransportLayerProtocol::
                                         from_string(array_elem[constants::EthernetSwitch::PROTOCOL].as_string()),
                                         array_elem[constants::EthernetSwitch::PORT].as_uint(),
                                         array_elem[constants::EthernetSwitch::PRIORITY].as_uint()});
                }
            }

            attributes.set_value(agent_framework::model::literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL, qap_array.to_json());
        }

        if (dcbx_config.is_member(constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosPriorityGroupMapping> qpm_array{};

            if (!dcbx_config[constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING].is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING]) {
                    qpm_array.add_entry({array_elem[constants::EthernetSwitch::PRIORITY_GROUP].as_uint(),
                                         array_elem[constants::EthernetSwitch::PRIORITY].as_uint()});
                }
            }

            attributes.set_value(agent_framework::model::literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING, qpm_array.to_json());
        }

        if (dcbx_config.is_member(constants::EthernetSwitch::BANDWIDTH_ALLOCATION)) {
            agent_framework::model::attribute::Array<agent_framework::model::attribute::QosBandwidthAllocation> qba_array{};

            if (!dcbx_config[constants::EthernetSwitch::BANDWIDTH_ALLOCATION].is_null()) {
                for (const auto& array_elem : dcbx_config[constants::EthernetSwitch::BANDWIDTH_ALLOCATION]) {
                    qba_array.add_entry({array_elem[constants::EthernetSwitch::PRIORITY_GROUP].as_uint(),
                                         array_elem[constants::EthernetSwitch::BANDWIDTH_PERCENT].as_uint()});
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
    auto s = model::Find<agent_framework::model::EthernetSwitch>(request.params[PathParam::ETHERNET_SWITCH_ID]).get();

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

    json::Value manager;
    manager[Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Manager, s.get_parent_uuid());

    r[Common::LINKS][Common::MANAGED_BY].push_back(std::move(manager));

    r[constants::EthernetSwitch::PFC_ENABLED] = s.get_pfc_enabled();
    r[constants::EthernetSwitch::LLDP_ENABLED] = s.get_lldp_enabled();
    r[constants::EthernetSwitch::ETS_ENABLED] = s.get_ets_enabled();
    r[constants::EthernetSwitch::DCBX_ENABLED] = s.get_dcbx_enabled();

    for (const auto& application_protocol : s.get_qos_application_protocol()) {
        json::Value array_elem(json::Value::Type::OBJECT);
        array_elem[constants::EthernetSwitch::PRIORITY] = application_protocol.get_priority();
        array_elem[constants::EthernetSwitch::PROTOCOL] = application_protocol.get_protocol();
        array_elem[constants::EthernetSwitch::PORT] = application_protocol.get_port();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::APPLICATION_PROTOCOL].
            push_back(std::move(array_elem));
    }

    for (const auto& priority_mapping : s.get_qos_priority_group_mapping()) {
        json::Value array_elem(json::Value::Type::OBJECT);
        array_elem[constants::EthernetSwitch::PRIORITY_GROUP] = priority_mapping.get_priority_group();
        array_elem[constants::EthernetSwitch::PRIORITY] = priority_mapping.get_priority();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::PRIORITY_TO_PRIORITY_GROUP_MAPPING].
            push_back(std::move(array_elem));
    }

    for (const auto& bandwidth_mapping : s.get_qos_bandwidth_allocation()) {
        json::Value array_elem(json::Value::Type::OBJECT);
        array_elem[constants::EthernetSwitch::PRIORITY_GROUP] = bandwidth_mapping.get_priority_group();
        array_elem[constants::EthernetSwitch::BANDWIDTH_PERCENT] = bandwidth_mapping.get_bandwidth_percent();

        r[constants::EthernetSwitch::DCBX_SHARED_CONFIGURATION][constants::EthernetSwitch::BANDWIDTH_ALLOCATION].
            push_back(std::move(array_elem));
    }

    set_response(response, r);
}

void endpoint::EthernetSwitch::patch(const server::Request& request, server::Response& response) {
    using namespace agent_framework::model;
    auto json = JsonValidator::validate_request_body<schema::EthernetSwitchPatchSchema>(request);
    auto attributes = fill_attributes(json);

    if (!attributes.empty()) {
        auto ethernet_switch = psme::rest::model::Find<agent_framework::model::EthernetSwitch>(
            request.params[PathParam::ETHERNET_SWITCH_ID]).get();

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

        gami_agent->execute_in_transaction(patch_ethernet_switch);
    }
    get(request, response);
}
