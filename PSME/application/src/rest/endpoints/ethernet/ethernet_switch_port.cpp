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

#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/utils/lag_utils.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/ethernet_switch_port.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/endpoints/endpoints.hpp"

#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/responses/network.hpp"
#include "agent-framework/module/responses/common.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::error;
using namespace psme::rest::utils;
using namespace psme::rest::validators;
using namespace agent_framework::module;
using namespace agent_framework::model;

using PatchMembersRequest = std::tuple<requests::AddEthernetSwitchPortMembers, requests::DeleteEthernetSwitchPortMembers>;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitchPort.EthernetSwitchPort";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchPort.v1_1_0.EthernetSwitchPort";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Ethernet Switch Port";
    r[Common::DESCRIPTION] = "Ethernet Switch Port description";
    r[constants::EthernetSwitchPort::PORT_ID] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[constants::EthernetSwitchPort::LINK_TYPE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::OPERATIONAL_STATE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::ADMINISTRATIVE_STATE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::LINK_SPEED] = json::Value::Type::NIL;

    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::SWITCH_ID] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::PORT_ID] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::CABLE_ID] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::NEIGHBOR_MAC] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::FRAME_SIZE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::AUTOSENSE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::FULL_DUPLEX] = json::Value::Type::NIL;
    r[constants::Common::MAC_ADDRESS] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::PORT_CLASS] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::PORT_MODE] = json::Value::Type::NIL;
    r[constants::EthernetSwitchPort::PORT_TYPE] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;

    r[constants::EthernetSwitchPort::IPv4_ADDRESSES] = json::Value::Type::ARRAY;
    r[constants::EthernetSwitchPort::IPv6_ADDRESSES] = json::Value::Type::ARRAY;
    r[constants::EthernetSwitchPort::VLANS] = json::Value::Type::OBJECT;
    r[constants::EthernetSwitchPort::STATIC_MACS] = json::Value::Type::OBJECT;
    r[constants::EthernetSwitchPort::NEIGHBOR_INTERFACE] = json::Value::Type::NIL;

    json::Value links;
    links[constants::EthernetSwitchPort::PRIMARY_VLAN] = json::Value::Type::NIL;
    links[constants::EthernetSwitchPort::SWITCH] = json::Value::Type::NIL;
    links[constants::EthernetSwitchPort::MEMBER_OF_PORT] = json::Value::Type::NIL;
    links[constants::EthernetSwitchPort::PORT_MEMBERS] = json::Value::Type::ARRAY;
    links[constants::EthernetSwitchPort::ACTIVE_ACLS] = json::Value::Type::ARRAY;

    r[Common::LINKS] = std::move(links);

    return r;
}


std::string get_switch(const server::Request& req) {
    return endpoint::PathBuilder(PathParam::BASE_URL)
        .append(Root::ETHERNET_SWITCHES)
        .append(req.params[PathParam::ETHERNET_SWITCH_ID])
        .build();
}


void add_member_of_port_link(json::Value& json, const std::string& port,
                             const std::string& switchID) {

    json[Common::LINKS][constants::EthernetSwitchPort::MEMBER_OF_PORT] =
        json::Value::Type::NIL;

    // return if the port is not a member
    const auto& port_members_manager =
        NetworkComponents::get_instance()->get_port_members_manager();
    if (!port_members_manager.child_exists(port)) {
        return;
    }

    // find parent
    const auto& port_manager = NetworkComponents::get_instance()->get_port_manager();
    auto parents = port_members_manager.get_parents(port);

    // safety check, if there is a parent, there should be only one
    if (1 != parents.size()) {
        log_error(GET_LOGGER("rest"), "Model Port/MemberOfPort link error");
        log_error(GET_LOGGER("rest"), "Port " << port <<
                                              " is used by more than one ports!");
        return;
    }

    const auto& parent = parents.front();
    try {
        // convert UUID into ID and fill the link
        const auto& parent_port_id = port_manager.get_entry(parent).get_id();
        json[Common::LINKS]
        [constants::EthernetSwitchPort::MEMBER_OF_PORT]
        [Common::ODATA_ID] = endpoint::PathBuilder(switchID)
            .append(constants::EthernetSwitch::PORTS)
            .append(parent_port_id).build();
    }
    catch (const agent_framework::exceptions::InvalidUuid&) {
        log_error(GET_LOGGER("rest"), "Model Port/MemberOfPort link error");
        log_error(GET_LOGGER("rest"), "Port " << parent <<
                                              " is present in the PortMembers table but it does not exist as a resource");
    }
}


void add_port_members_links(json::Value& json, const std::string& port,
                            const std::string& switchID) {

    json[Common::LINKS][constants::EthernetSwitchPort::PORT_MEMBERS] =
        json::Value::Type::ARRAY;

    // return if the port is not a LAG
    const auto& port_members_manager =
        NetworkComponents::get_instance()->get_port_members_manager();
    if (!port_members_manager.parent_exists(port)) {
        return;
    }

    // find children
    const auto& port_manager = NetworkComponents::get_instance()->get_port_manager();
    auto children = port_members_manager.get_children(port);

    // fill links
    for (const auto& child : children) {
        try {
            const auto& child_port = port_manager.get_entry(child);
            json::Value link;
            link[Common::ODATA_ID] = endpoint::PathBuilder(switchID)
                .append(constants::EthernetSwitch::PORTS)
                .append(child_port.get_id()).build();
            json[Common::LINKS][constants::EthernetSwitchPort::PORT_MEMBERS]
                .push_back(link);
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model Port/PortMembers link error");
            log_error(GET_LOGGER("rest"), "Port " << child <<
                                                  " is present in the PortMembers table but it does not exist"
                                                      " as a resource");
        }
    }
}


void add_active_acls_links(json::Value& json, const std::string& port) {
    const auto& acls = NetworkComponents::get_instance()->
        get_acl_port_manager().get_parents(port);

    for (const auto& acl : acls) {
        try {
            json::Value link;
            link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                enums::Component::Acl, acl);
            json[Common::LINKS][constants::EthernetSwitchPort::ACTIVE_ACLS].
                push_back(std::move(link));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model Port/ActiveACLs link error");
            log_error(GET_LOGGER("rest"), "ACL " << acl <<
                                                 " is present in the ActiveACLs table but it does not exist"
                                                     " as a resource");
        }
    }
}


void add_primary_vlan_link(json::Value& json, const OptionalField<std::string>& vlan, const std::string& url) {
    json[Common::LINKS][constants::EthernetSwitchPort::PRIMARY_VLAN] = json::Value::Type::NIL;

    // if empty -> set to empty link and return
    if (!vlan) {
        return;
    }

    const auto& vlan_manager =
        NetworkComponents::get_instance()->get_port_vlan_manager();
    try {
        auto vlan_id = vlan_manager.get_entry(vlan).get_id();
        json[Common::LINKS][constants::EthernetSwitchPort::PRIMARY_VLAN][Common::ODATA_ID] =
            endpoint::PathBuilder(url).append(constants::EthernetSwitchPort::VLANS).append(vlan_id).build();
    }
    catch (agent_framework::exceptions::InvalidUuid&) {
        log_error(GET_LOGGER("rest"), "Model Port/PrimaryVLAN link error.");
        log_error(GET_LOGGER("rest"), "Primary VLAN " << vlan << " does not exist but is used by a port.");
    }
}


PatchMembersRequest prepare_patch_members_requests(const std::string& port_uuid,
                                                   const std::vector<std::string> requested_members) {
    std::vector<std::string> members_to_add;
    std::vector<std::string> members_to_remove;
    psme::rest::endpoint::utils::children_to_add_to_remove(NetworkComponents::get_instance()->
        get_port_members_manager(), port_uuid, requested_members, members_to_add, members_to_remove);

    auto remove_request = requests::DeleteEthernetSwitchPortMembers(
        members_to_remove, port_uuid, attribute::Oem());

    auto add_request = requests::AddEthernetSwitchPortMembers(
        members_to_add, port_uuid, attribute::Oem());

    return PatchMembersRequest(add_request, remove_request);
}


void execute_patch_members(const agent_framework::model::EthernetSwitchPort& port,
                           psme::core::agent::JsonAgentSPtr gami_agent,
                           const PatchMembersRequest& patch_members_requests) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    const auto& add_request = std::get<0>(patch_members_requests);
    const auto& remove_request = std::get<1>(patch_members_requests);

    if (add_request.get_members().size() > 0) {
        gami_agent->execute<responses::AddEthernetSwitchPortMembers>(add_request);

        // Update info about the added member ports
        for (const auto& member : add_request.get_members()) {
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load(gami_agent, port.get_parent_uuid(), enums::Component::EthernetSwitch, member, false);
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load_collection(gami_agent, member,
                                enums::Component::EthernetSwitchPort,
                                enums::CollectionType::EthernetSwitchPortVlans, false);
        }
    }

    if (remove_request.get_members().size() > 0) {
        gami_agent->execute<responses::DeleteEthernetSwitchPortMembers>(remove_request);

        // Update info about the deleted member ports
        for (const auto& member : remove_request.get_members()) {
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load(gami_agent,
                     port.get_parent_uuid(),
                     enums::Component::EthernetSwitch,
                     member,
                     false);
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load_collection(gami_agent,
                                member,
                                enums::Component::EthernetSwitchPort,
                                enums::CollectionType::EthernetSwitchPortVlans, false);
        }
    }
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::EthernetSwitchPort::LINK_SPEED_MBPS,      constants::EthernetSwitchPort::LINK_SPEED},
    {agent_framework::model::literals::EthernetSwitchPort::ADMINISTRATIVE_STATE, constants::EthernetSwitchPort::ADMINISTRATIVE_STATE},
    {agent_framework::model::literals::EthernetSwitchPort::FRAME_SIZE,           constants::EthernetSwitchPort::FRAME_SIZE},
    {agent_framework::model::literals::EthernetSwitchPort::AUTO_SENSE,           constants::EthernetSwitchPort::AUTOSENSE},
    {agent_framework::model::literals::EthernetSwitchPort::DEFAULT_VLAN,         constants::EthernetSwitchPort::PRIMARY_VLAN}
};


attribute::Attributes fill_attributes(json::Value& json) {
    attribute::Attributes attributes{};

    if (json.is_member(constants::EthernetSwitchPort::LINK_SPEED)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitchPort::LINK_SPEED_MBPS,
                             json[constants::EthernetSwitchPort::LINK_SPEED].as_int64());
    }
    if (json.is_member(constants::EthernetSwitchPort::ADMINISTRATIVE_STATE)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitchPort::ADMINISTRATIVE_STATE,
                             json[constants::EthernetSwitchPort::ADMINISTRATIVE_STATE].as_string());
    }
    if (json.is_member(constants::EthernetSwitchPort::FRAME_SIZE)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitchPort::FRAME_SIZE,
                             json[constants::EthernetSwitchPort::FRAME_SIZE].as_int64());
    }
    if (json.is_member(constants::EthernetSwitchPort::AUTOSENSE)) {
        attributes.set_value(agent_framework::model::literals::EthernetSwitchPort::AUTO_SENSE,
                             json[constants::EthernetSwitchPort::AUTOSENSE].as_bool());
    }
    if (json[Common::LINKS].is_member(constants::EthernetSwitchPort::PRIMARY_VLAN)) {
        try {
            const auto& primary_vlan_url =
                json[Common::LINKS][constants::EthernetSwitchPort::PRIMARY_VLAN][Common::ODATA_ID].as_string();
            auto params = server::Multiplexer::get_instance()->
                get_params(primary_vlan_url, constants::Routes::VLAN_NETWORK_INTERFACE_PATH);

            auto pvid =
                psme::rest::model::Find<agent_framework::model::EthernetSwitchPortVlan>(params[PathParam::VLAN_ID])
                    .via<agent_framework::model::EthernetSwitch>(params[PathParam::ETHERNET_SWITCH_ID])
                    .via<agent_framework::model::EthernetSwitchPort>(params[PathParam::SWITCH_PORT_ID])
                    .get_one();

            attributes.set_value(agent_framework::model::literals::EthernetSwitchPort::DEFAULT_VLAN, pvid->get_uuid());
        }
        catch (const agent_framework::exceptions::NotFound& ex) {
            throw agent_framework::exceptions::InvalidValue("Cannot patch default VLAN: " + ex.get_message());
        }
    }

    return attributes;
}
}


endpoint::EthernetSwitchPort::EthernetSwitchPort(const std::string& path) : EndpointBase(path) {}


endpoint::EthernetSwitchPort::~EthernetSwitchPort() {}


void endpoint::EthernetSwitchPort::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::SWITCH_PORT_ID];
    r[Common::NAME] = constants::EthernetSwitchPort::PORT + req.params[PathParam::SWITCH_PORT_ID];

    auto port =
        psme::rest::model::Find<agent_framework::model::EthernetSwitchPort>(req.params[PathParam::SWITCH_PORT_ID])
            .via<agent_framework::model::EthernetSwitch>(req.params[PathParam::ETHERNET_SWITCH_ID])
            .get();

    r[constants::EthernetSwitchPort::PORT_ID] = port.get_port_identifier();
    endpoint::status_to_json(port, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::HealthRollup<agent_framework::model::EthernetSwitchPort>().get(port.get_uuid());

    r[constants::EthernetSwitchPort::LINK_TYPE] = port.get_link_technology();
    r[constants::EthernetSwitchPort::OPERATIONAL_STATE] = port.get_operational_state();
    r[constants::EthernetSwitchPort::ADMINISTRATIVE_STATE] = port.get_administrative_state();
    r[constants::EthernetSwitchPort::LINK_SPEED] = port.get_link_speed_mbps();

    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::SWITCH_ID] =
        port.get_neighbor_info().get_switch_identifier();
    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::PORT_ID] =
        port.get_neighbor_info().get_port_identifier();
    r[constants::EthernetSwitchPort::NEIGHBOR_INFO][constants::EthernetSwitchPort::CABLE_ID] =
        port.get_neighbor_info().get_cable_id();

    r[constants::EthernetSwitchPort::NEIGHBOR_MAC] = port.get_neighbor_mac();
    r[constants::EthernetSwitchPort::FRAME_SIZE] = port.get_frame_size();
    r[constants::EthernetSwitchPort::AUTOSENSE] = port.get_auto_sense();
    r[constants::EthernetSwitchPort::FULL_DUPLEX] = port.get_full_duplex();
    r[constants::Common::MAC_ADDRESS] = port.get_mac_address();
    r[constants::EthernetSwitchPort::PORT_CLASS] = port.get_port_class();
    r[constants::EthernetSwitchPort::PORT_MODE] = port.get_port_mode();
    r[constants::EthernetSwitchPort::PORT_TYPE] = port.get_port_type();

    const auto ipv4_addr = port.get_ipv4_address();
    if (static_cast<bool>(ipv4_addr.get_address())) {
        json::Value ipv4_address;
        ipv4_address[IpAddress::ADDRESS] = ipv4_addr.get_address();
        ipv4_address[IpAddress::SUBNET_MASK] = ipv4_addr.get_subnet_mask();
        ipv4_address[IpAddress::ADDRESS_ORIGIN] = ipv4_addr.get_address_origin();
        ipv4_address[IpAddress::GATEWAY] = ipv4_addr.get_gateway();
        r[constants::EthernetSwitchPort::IPv4_ADDRESSES].push_back(std::move(ipv4_address));
    }

    const auto ipv6_addr = port.get_ipv6_address();
    if (static_cast<bool>(ipv6_addr.get_address())) {
        json::Value ipv6_address;
        ipv6_address[IpAddress::ADDRESS] = ipv6_addr.get_address();
        // in GAMI there is DHCP option which has to be shown as DHCPv6
        auto addr_origin = ipv6_addr.get_address_origin();
        ipv6_address[IpAddress::ADDRESS_ORIGIN] =
            addr_origin == enums::Ipv6AddressOrigin::DHCP ?
            json::Value("DHCPv6") : json::Value(addr_origin);
        ipv6_address[IpAddress::ADDRESS_STATE] =
            ipv6_addr.get_address_state();
        ipv6_address[IpAddress::PREFIX_LENGTH] = ipv6_addr.get_prefix_length();
        r[constants::EthernetSwitchPort::IPv6_ADDRESSES].push_back(
            std::move(ipv6_address));
    }

    r[constants::EthernetSwitchPort::VLANS][Common::ODATA_ID] =
        PathBuilder(req).append(constants::EthernetSwitchPort::VLANS).build();
    r[constants::EthernetSwitchPort::STATIC_MACS][Common::ODATA_ID] =
        PathBuilder(req).append(constants::EthernetSwitchPort::STATIC_MACS).build();
    r[Common::LINKS][constants::EthernetSwitchPort::SWITCH][Common::ODATA_ID] =
        get_switch(req);

    add_primary_vlan_link(r, port.get_default_vlan(), PathBuilder(req).build());
    add_member_of_port_link(r, port.get_uuid(), get_switch(req));
    add_port_members_links(r, port.get_uuid(), get_switch(req));
    add_active_acls_links(r, port.get_uuid());

    set_response(res, r);
}


void endpoint::EthernetSwitchPort::patch(const server::Request& request, server::Response& response) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto json = JsonValidator::validate_request_body<schema::EthernetSwitchPortPatchSchema>(request);
    auto attributes = fill_attributes(json);

    // Holding reference to parent object ensures that locks are acquired in the same order in each thread
    auto parent_switch = psme::rest::model::Find<agent_framework::model::EthernetSwitch>(
        request.params[PathParam::ETHERNET_SWITCH_ID]).get();
    auto port = psme::rest::model::Find<agent_framework::model::EthernetSwitchPort>(
        request.params[PathParam::SWITCH_PORT_ID])
        .via<agent_framework::model::EthernetSwitch>(request.params[PathParam::ETHERNET_SWITCH_ID])
        .get();

    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(port.get_agent_id());
    std::vector<std::string> port_members{};

    if (json[Common::LINKS].is_member(constants::EthernetSwitchPort::PORT_MEMBERS)) {
        LagUtils::validate_is_logical(port.get_port_class().value());
        port_members = LagUtils::get_port_members(json);
        LagUtils::validate_port_members(port_members, port.get_uuid());
    }

    auto patch_port = [&, gami_agent] {
        bool updated{false};

        if (!port_members.empty()) {
            const auto& patch_members_request = prepare_patch_members_requests(port.get_uuid(), port_members);
            execute_patch_members(port, gami_agent, patch_members_request);
            updated = true;
        }

        if (!attributes.empty()) {
            const auto& set_component_attributes_request = requests::SetComponentAttributes{port.get_uuid(),
                                                                                            attributes};
            const auto& set_component_attributes_response =
                gami_agent->execute<responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw ServerException(error);
            }

            updated = true;
        }

        if (updated) {
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load(gami_agent,
                     parent_switch.get_uuid(), agent_framework::model::enums::Component::EthernetSwitch,
                     port.get_uuid(), true);
        }
    };

    gami_agent->execute_in_transaction(patch_port);

    get(request, response);
}


void endpoint::EthernetSwitchPort::del(const server::Request& req, server::Response& res) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto port = psme::rest::model::Find
        <agent_framework::model::EthernetSwitchPort>(req.params[PathParam::SWITCH_PORT_ID]).via
        <agent_framework::model::EthernetSwitch>(req.params[PathParam::ETHERNET_SWITCH_ID]).get();

    auto gami_req = requests::DeleteEthernetSwitchPort(port.get_uuid());

    const auto port_members = NetworkComponents::get_instance()->get_port_members_manager().get_children(
        port.get_uuid());
    const auto switch_uuid = port.get_parent_uuid();

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(port.get_agent_id());

    auto remove_port = [&, gami_agent] {
        // try removing port from agent's model
        gami_agent->execute<responses::DeleteEthernetSwitchPort>(gami_req);

        // remove the resource
        HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->remove(port.get_uuid());

        // Update our information about member ports and their PortVlans
        for (const auto& member : port_members) {
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load(gami_agent,
                     switch_uuid,
                     enums::Component::EthernetSwitch,
                     member,
                     false);
            HandlerManager::get_instance()->get_handler(enums::Component::EthernetSwitchPort)->
                load_collection(gami_agent,
                                member,
                                enums::Component::EthernetSwitchPort,
                                enums::CollectionType::EthernetSwitchPortVlans,
                                false);
        }
    };

    gami_agent->execute_in_transaction(remove_port);

    res.set_status(server::status_2XX::NO_CONTENT);
}
