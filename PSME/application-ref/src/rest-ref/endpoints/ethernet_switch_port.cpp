/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/endpoints/ethernet_switch_port.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/utils/lag_utils.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"

#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/requests/network.hpp"
#include "agent-framework/module-ref/responses/network.hpp"
#include "agent-framework/module-ref/requests/common.hpp"
#include "agent-framework/module-ref/responses/common.hpp"
#include "agent-framework/module-ref/constants/network.hpp"

#include <json/json.hpp>
#include <regex>

using namespace psme::rest;
using namespace psme::rest::utils;
using namespace psme::rest::error;
using namespace app::rest;
using namespace app::rest::constants;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitches/Members/__SWITCH_ID__/Ports/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitchPort.1.0.0.EthernetSwitchPort";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Ethernet Switch Port";
    r[Common::DESCRIPTION] = "Ethernet Switch Port description";
    r[constants::SwitchPort::PORT_ID] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[constants::SwitchPort::LINK_TYPE] = json::Value::Type::NIL;
    r[constants::SwitchPort::OPERATIONAL_STATE] = json::Value::Type::NIL;
    r[constants::SwitchPort::ADMINISTRATIVE_STATE] = json::Value::Type::NIL;
    r[constants::SwitchPort::LINK_SPEED] = json::Value::Type::NIL;

    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::SWITCH_ID] = json::Value::Type::NIL;
    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::PORT_ID] = json::Value::Type::NIL;
    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::CABLE_ID] = json::Value::Type::NIL;
    r[constants::SwitchPort::NEIGHBOR_MAC] = json::Value::Type::NIL;
    r[constants::SwitchPort::FRAME_SIZE] = json::Value::Type::NIL;
    r[constants::SwitchPort::AUTOSENSE] = json::Value::Type::NIL;
    r[constants::SwitchPort::FULL_DUPLEX] = json::Value::Type::NIL;
    r[constants::SwitchPort::MAC_ADDRESS] = json::Value::Type::NIL;
    r[constants::SwitchPort::PORT_CLASS] = json::Value::Type::NIL;
    r[constants::SwitchPort::PORT_MODE] = json::Value::Type::NIL;
    r[constants::SwitchPort::PORT_TYPE] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;

    r[constants::SwitchPort::IPv4_ADDRESSES] = json::Value::Type::ARRAY;
    r[constants::SwitchPort::IPv6_ADDRESSES] = json::Value::Type::ARRAY;
    r[constants::SwitchPort::VLANS] = json::Value::Type::OBJECT;
    r[constants::SwitchPort::STATIC_MACS] = json::Value::Type::OBJECT;

    json::Value links;
    links[constants::SwitchPort::PRIMARY_VLAN] =
        json::Value::Type::NIL;
    links[constants::SwitchPort::SWITCH] =
        json::Value::Type::NIL;
    links[constants::SwitchPort::MEMBER_OF_PORT] =
        json::Value::Type::NIL;
    links[constants::SwitchPort::PORT_MEMBERS] =
        json::Value::Type::ARRAY;
    links[constants::SwitchPort::ACTIVE_ACLS] =
        json::Value::Type::ARRAY;
    links[Common::OEM] = json::Value::Type::OBJECT;

    links[Common::OEM][Common::RACKSCALE]
        [Common::ODATA_TYPE] =
            "#Intel.Oem.EthernetSwitchPort";
    links[Common::OEM][Common::RACKSCALE]
        [constants::SwitchPort::NEIGHBOR_INTERFACE][Common::ODATA_ID] =
            json::Value::Type::NIL;

    r[Common::LINKS] = std::move(links);

    return r;
}

std::string get_switch(const server::Request& req){
    return endpoint::PathBuilder(PathParam::BASE_URL)
                .append(Root::ETHERNET_SWITCHES)
                .append(req.params[PathParam::ETHERNET_SWITCH_ID])
                .build();
}

void add_member_of_port_link(json::Value& json, const std::string& port,
    const std::string& switchID) {

    json[Common::LINKS][constants::SwitchPort::MEMBER_OF_PORT] =
        json::Value::Type::NIL;

    // return if the port is not a member
    const auto& port_members_manager =
        NetworkManager::get_instance()->get_port_members_manager();
    if (!port_members_manager.child_exists(port)) {
        return ;
    }

    // find parent
    const auto& port_manager = NetworkManager::get_instance()->get_port_manager();
    auto parents = port_members_manager.get_parents(port);

    // safety check, if there is a parent, there should be only one
    if (1 != parents.size()) {
        log_error(GET_LOGGER("rest"), "Model Port/MemberOfPort link error");
        log_error(GET_LOGGER("rest"), "Port " << port <<
            " is used by more than one ports!");
        return ;
    }

    const auto& parent = parents.front();
    try {
        // convert UUID into ID and fill the link
        const auto& parent_port_id = port_manager.get_entry(parent).get_id();
        json[Common::LINKS]
            [constants::SwitchPort::MEMBER_OF_PORT]
            [Common::ODATA_ID] = endpoint::PathBuilder(switchID)
                                    .append(constants::Switch::PORTS)
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

    json[Common::LINKS][constants::SwitchPort::PORT_MEMBERS] =
        json::Value::Type::ARRAY;

    // return if the port is not a LAG
    const auto& port_members_manager =
        NetworkManager::get_instance()->get_port_members_manager();
    if (!port_members_manager.parent_exists(port)) {
        return ;
    }

    // find children
    const auto& port_manager = NetworkManager::get_instance()->get_port_manager();
    auto children = port_members_manager.get_children(port);

    // fill links
    for (const auto& child : children) {
        try {
            const auto& child_port = port_manager.get_entry(child);
            json::Value link;
            link[Common::ODATA_ID] = endpoint::PathBuilder(switchID)
                                        .append(constants::Switch::PORTS)
                                        .append(child_port.get_id()).build();
            json[Common::LINKS][constants::SwitchPort::PORT_MEMBERS]
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
    const auto& acls = NetworkManager::get_instance()->
        get_acl_port_manager().get_parents(port);

    for (const auto& acl : acls) {
        try {
            json::Value link;
            link[Common::ODATA_ID] = endpoint::utils::get_component_url(
                enums::Component::Acl, acl);
            json[Common::LINKS][constants::SwitchPort::ACTIVE_ACLS].
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

void add_primary_vlan_link(json::Value& json, const OptionalField<std::string>& vlan,
    const std::string& url) {

      json[Common::LINKS][constants::SwitchPort::PRIMARY_VLAN] =
          json::Value::Type::NIL;

    // if empty -> set to empty link and return
    if (!vlan) {
        return ;
    }

    const auto& vlan_manager =
        NetworkManager::get_instance()->get_port_vlan_manager();
    try {
        auto vlan_id = vlan_manager.get_entry(vlan).get_id();
        json[Common::LINKS][constants::SwitchPort::PRIMARY_VLAN][Common::ODATA_ID] =
            endpoint::PathBuilder(url).append(constants::SwitchPort::VLANS)
                                      .append(vlan_id).build();
    }
    catch (agent_framework::exceptions::InvalidUuid&) {
        log_error(GET_LOGGER("rest"), "Model Port/PrimaryVLAN link error");
        log_error(GET_LOGGER("rest"), "Primary VLAN " << std::string(vlan) <<
            " does not exist but is used by a port");
    }
}

json::Value validate_patch_request(const server::Request& request) {
    using namespace agent_framework::model::enums;
    using json::Pair;

    json::Value administrative_states{json::Value::Type::ARRAY};
    for(const auto& value : AdministrativeState::get_values()) {
        administrative_states.push_back(value);
    }

    json::Value schema({
        Pair(constants::SwitchPort::ADMINISTRATIVE_STATE,
            JsonValidator::any_of(
                JsonValidator::STRING_TYPE, administrative_states)),
        Pair(constants::SwitchPort::LINK_SPEED,
            JsonValidator::type_of(JsonValidator::UINT_TYPE)),
        Pair(constants::SwitchPort::FRAME_SIZE,
            JsonValidator::type_of(JsonValidator::UINT_TYPE)),
        Pair(constants::SwitchPort::AUTOSENSE,
            JsonValidator::type_of(JsonValidator::BOOL_TYPE)),
        Pair(Common::LINKS, json::Value({
            Pair(constants::SwitchPort::PORT_MEMBERS,
                JsonValidator::type_of(JsonValidator::ARRAY_TYPE))
        }))
    });

    return JsonValidator::validate_request_body(request, schema);
}

void execute_patch_members(const std::string& port_uuid, const std::string& agent_id,
        const std::vector<std::string> requested_members) {
    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto& members_manager =
        NetworkManager::get_instance()->get_port_members_manager();

    std::vector<std::string> members_to_remove = members_manager
                                                 .get_children(port_uuid);

    std::vector<std::string> members_to_add;
    for (const auto& requested_member : requested_members) {
        auto iterator = std::remove(members_to_remove.begin(),
                                  members_to_remove.end(),
                                  requested_member);
        if (iterator != members_to_remove.end()) {
            members_to_remove.erase(iterator, members_to_remove.end());
        }
        else {
            members_to_add.emplace_back(requested_member);
        }
    }

    auto rem_req = requests::DeleteEthernetSwitchPortMembers(
        members_to_remove, port_uuid, attribute::Oem());

    auto add_req = requests::AddEthernetSwitchPortMembers(
        members_to_add, port_uuid, attribute::Oem());

    const auto switch_uuid = NetworkManager::get_instance()->get_port_manager().get_entry_reference(port_uuid)
            ->get_parent_uuid();

    AgentManager::get_instance()->call_method
        <responses::AddEthernetSwitchPortMembers>
        (agent_id, add_req);

    // Update info about the added member ports
    for (const auto& member : members_to_add) {
        HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load(AgentManager::get_instance()->get_agent(agent_id), switch_uuid, member, false);
	HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load_collection(AgentManager::get_instance()->get_agent(agent_id), member, enums::CollectionType::PortVlans, false);
    }

    AgentManager::get_instance()->call_method
        <responses::DeleteEthernetSwitchPortMembers>
        (agent_id, rem_req);

    // Update info about the deleted member ports
    for (const auto& member : members_to_remove) {
        HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load(AgentManager::get_instance()->get_agent(agent_id), switch_uuid, member, false);
	HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load_collection(AgentManager::get_instance()->get_agent(agent_id), member, enums::CollectionType::PortVlans, false);
    }
}

attribute::Attributes fill_attributes(json::Value& json) {
    attribute::Attributes attributes{};

    if (json.is_member(constants::SwitchPort::LINK_SPEED)) {
        attributes.set_value(agent_framework::model::literals::SwitchPort::LINK_SPEED_MBPS,
                json[constants::SwitchPort::LINK_SPEED].as_uint());
    }
    if (json.is_member(constants::SwitchPort::ADMINISTRATIVE_STATE)) {
        attributes.set_value(agent_framework::model::literals::SwitchPort::ADMINISTRATIVE_STATE,
                json[constants::SwitchPort::ADMINISTRATIVE_STATE].as_string());
    }
    if (json.is_member(constants::SwitchPort::FRAME_SIZE)) {
        attributes.set_value(agent_framework::model::literals::SwitchPort::FRAME_SIZE,
                json[constants::SwitchPort::FRAME_SIZE].as_uint());
    }
    if (json.is_member(constants::SwitchPort::AUTOSENSE)) {
        attributes.set_value(agent_framework::model::literals::SwitchPort::AUTO_SENSE,
                json[constants::SwitchPort::AUTOSENSE].as_bool());
    }
    if (json[Common::LINKS].is_member(constants::SwitchPort::PRIMARY_VLAN)) {
        try {
            auto params = psme::rest::model::Mapper::get_params(
                json[Common::LINKS][constants::SwitchPort::PRIMARY_VLAN]
                    [Common::ODATA_ID].as_string());
            auto pvid = psme::rest::model::Find
                    <agent_framework::model::PortVlan>(params[PathParam::VLAN_ID]).via
                    <agent_framework::model::Switch>(params[PathParam::ETHERNET_SWITCH_ID]).via
                    <agent_framework::model::SwitchPort>(params[PathParam::SWITCH_PORT_ID]).get_one();
            attributes.set_value
                (agent_framework::model::literals::SwitchPort::DEFAULT_VLAN,
                    pvid->get_uuid());
        } catch (const std::exception& ex) {
            throw(agent_framework::exceptions::InvalidParameters(
                    std::string("Cannot patch default VLAN: ") + ex.what()));
        }
    }

    return attributes;
}
}

endpoint::EthernetSwitchPort::EthernetSwitchPort(const std::string& path) : Endpoint(path) {}
endpoint::EthernetSwitchPort::~EthernetSwitchPort() {}

void endpoint::EthernetSwitchPort::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    auto switch_id = psme::rest::model::Find<agent_framework::model::Switch>(
        req.params[PathParam::ETHERNET_SWITCH_ID]).get_one()->get_id();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SWITCH_ID__"), std::to_string(switch_id));
    r[Common::ID] = req.params[PathParam::SWITCH_PORT_ID];
    r[Common::NAME] = constants::SwitchPort::PORT +
        req.params[PathParam::SWITCH_PORT_ID];

    auto port =
        psme::rest::model::Find<agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID])
        .via<agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID])
        .get();

    r[constants::SwitchPort::PORT_ID] = port.get_port_identifier();
    endpoint::utils::status_to_json(port,r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::utils::rollup_switch_port_health(port.get_uuid()).to_string();

    r[constants::SwitchPort::LINK_TYPE] =
        port.get_link_technology();
    r[constants::SwitchPort::OPERATIONAL_STATE] =
        port.get_operational_state();
    r[constants::SwitchPort::ADMINISTRATIVE_STATE] =
        port.get_administrative_state();
    r[constants::SwitchPort::LINK_SPEED] =
        port.get_link_speed_mbps();

    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::SWITCH_ID] =
        port.get_neighbor_info().get_switch_identifier();
    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::PORT_ID] =
        port.get_neighbor_info().get_port_identifier();
    r[constants::SwitchPort::NEIGHBOR_INFO]
     [constants::SwitchPort::CABLE_ID] =
        port.get_neighbor_info().get_cable_id();

    r[constants::SwitchPort::NEIGHBOR_MAC] = port.get_neighbor_mac();
    r[constants::SwitchPort::FRAME_SIZE] = port.get_frame_size();
    r[constants::SwitchPort::AUTOSENSE] = port.get_auto_sense();
    r[constants::SwitchPort::FULL_DUPLEX] = port.get_full_duplex();
    r[constants::SwitchPort::MAC_ADDRESS] = port.get_mac_address();
    r[constants::SwitchPort::PORT_CLASS] =
        port.get_port_class();
    r[constants::SwitchPort::PORT_MODE] =
        port.get_port_mode();
    r[constants::SwitchPort::PORT_TYPE] =
        port.get_port_type();

    const auto ipv4_addr = port.get_ipv4_address();
    if (static_cast<bool>(ipv4_addr.get_address())) {
        json::Value ipv4_address;
        ipv4_address[IpAddress::ADDRESS] = ipv4_addr.get_address();
        ipv4_address[IpAddress::SUBNET_MASK] = ipv4_addr.get_subnet_mask();
        ipv4_address[IpAddress::ADDRESS_ORIGIN] =
            ipv4_addr.get_address_origin();
        ipv4_address[IpAddress::GATEWAY] = ipv4_addr.get_gateway();
        r[constants::SwitchPort::IPv4_ADDRESSES].push_back(
            std::move(ipv4_address));
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
        r[constants::SwitchPort::IPv6_ADDRESSES].push_back(
            std::move(ipv6_address));
    }

    r[constants::SwitchPort::VLANS][Common::ODATA_ID] =
        PathBuilder(req).append(constants::SwitchPort::VLANS).build();
    r[constants::SwitchPort::STATIC_MACS][Common::ODATA_ID] =
            PathBuilder(req).append(constants::SwitchPort::STATIC_MACS).build();
    r[Common::LINKS][constants::SwitchPort::SWITCH][Common::ODATA_ID] =
        get_switch(req);

    add_primary_vlan_link(r, port.get_default_vlan(), PathBuilder(req).build());
    add_member_of_port_link(r, port.get_uuid(), get_switch(req));
    add_port_members_links(r, port.get_uuid(), get_switch(req));
    add_active_acls_links(r, port.get_uuid());

    set_response(res, r);
}

void endpoint::EthernetSwitchPort::patch(const server::Request& req, server::Response& res) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;
    const auto& manager = psme::core::agent::AgentManager::get_instance();

    auto port = psme::rest::model::Find
    <agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID]).via
    <agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    auto json = validate_patch_request(req);
    auto attributes = fill_attributes(json);

    bool is_valid_parameters{false};
    if (json[Common::LINKS].is_member(constants::SwitchPort::PORT_MEMBERS)) {
        LagUtils::validate_is_logical(port->get_port_class().value());
        auto port_members = LagUtils::get_port_members(json);
        LagUtils::validate_port_members(port_members, port->get_uuid());
        execute_patch_members(port->get_uuid(), port->get_agent_id(), port_members);
        is_valid_parameters = true;
    }
    if (!attributes.to_json().isNull()) {
        requests::SetComponentAttributes
        set_component_attributes_request{port->get_uuid(), attributes};
        manager->call_method
            <responses::SetComponentAttributes>(
                port->get_agent_id(), set_component_attributes_request);

        is_valid_parameters = true;
    }
    if (!is_valid_parameters) {
        throw(agent_framework::exceptions::InvalidParameters(
            "Cannot patch Switch Port. "
            "Missing patch data (attributes to patch or new member list)"));
    }

    HandlerManager::get_instance()->get_handler(
    enums::Component::SwitchPort)->
    load(manager->get_agent(port->get_agent_id()), port->get_parent_uuid(),
            port->get_uuid(), true);

    res.set_status(server::status_2XX::NO_CONTENT);
}

void endpoint::EthernetSwitchPort::del(const server::Request& req, server::Response& res) {
    using AgentManager = psme::core::agent::AgentManager;
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto port = psme::rest::model::Find
        <agent_framework::model::SwitchPort>(req.params[PathParam::SWITCH_PORT_ID]).via
        <agent_framework::model::Switch>(req.params[PathParam::ETHERNET_SWITCH_ID]).get_one();

    auto gami_req = requests::DeleteEthernetSwitchPort(
        port->get_uuid());

    const auto port_members = NetworkManager::get_instance()->get_port_members_manager().get_children(port->get_uuid());
    const auto switch_uuid = port->get_parent_uuid();
    auto agent = AgentManager::get_instance()->get_agent(port->get_agent_id());

    // try removing port from agent's model
    AgentManager::get_instance()->call_method
        <responses::DeleteEthernetSwitchPort>
        (port->get_agent_id(), gami_req);

    // remove the resource, DO NOT use port reference after this line !!
    HandlerManager::get_instance()->get_handler(
        enums::Component::SwitchPort)->
        remove(port->get_uuid());

    // Update our information about member ports and their PortVlans
    for (const auto& member : port_members) {
        HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load(agent, switch_uuid, member, false);
        HandlerManager::get_instance()->get_handler(enums::Component::SwitchPort)->
                load_collection(agent, member, enums::CollectionType::PortVlans, false);
    }

    res.set_status(server::status_2XX::NO_CONTENT);
}
