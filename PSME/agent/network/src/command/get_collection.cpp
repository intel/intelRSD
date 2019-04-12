/*!
 * @brief Implementation of GetCollection command.
 *
 * File contains all implementations of methods for GetCollection command.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file get_collection.cpp
 */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "logger/logger.hpp"
//#include "api/remote_switch_info.hpp"
//#include "api/dcrp/remote_switch_info.hpp"
#include "utils/lag.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;
using namespace agent_framework::model::enums;
using namespace agent::network::utils;

namespace {
void get_member_collections(const GetCollection::Request& request,
                            GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    const auto& port_manager = network_components->get_port_manager();
    auto port = port_manager.get_entry(request.get_uuid());
    if (PortClass::Logical != port.get_port_class()) {
        THROW(agent_framework::exceptions::InvalidValue, "network-agent", "Port is not LAG.");
    }
    for (const auto& member_uuid : port_manager.get_keys()) {
        auto member = port_manager.get_entry(member_uuid);
        if (is_member_port(member.get_port_identifier())) {
            const auto lag_identifier =
                get_lag_identifier(member.get_port_identifier());
            if (port.get_port_identifier() == lag_identifier) {
                response.add_entry(member_uuid);
            }
        }
    }
}


void get_vlan_collections(const GetCollection::Request& request,
                          GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    const auto& port_manager = network_components->get_port_manager();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    auto port = port_manager.get_entry(request.get_uuid());
    for (const auto& uuid : port_vlan_manager.get_keys(port.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_port_collections(const GetCollection::Request& request,
                          GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    const auto& switch_manager = network_components->get_switch_manager();
    auto& port_manager = network_components->get_port_manager();
    auto switch_module = switch_manager.get_entry(request.get_uuid());
    for (const auto& uuid : port_manager.get_keys(switch_module.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_switch_collections(const GetCollection::Request& request,
                            GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    auto common_components = CommonComponents::get_instance();

    auto& manager = common_components->get_module_manager();
    auto& switch_manager = network_components->get_switch_manager();
    auto manager_model = manager.get_entry(request.get_uuid());
    for (const auto& switch_uuid :
        switch_manager.get_keys(manager_model.get_uuid())) {
        response.add_entry(switch_uuid);
    }
}

void get_neighbor_collections(const GetCollection::Request&,
                              GetCollection::Response&) {}

void get_chassis_collections(const GetCollection::Request& request,
                             GetCollection::Response& response) {
    auto common_components = CommonComponents::get_instance();

    auto& manager = common_components->get_module_manager();
    auto& chassis_manager = common_components->get_chassis_manager();
    auto manager_model = manager.get_entry(request.get_uuid());
    for (const auto& chassis_uuid :
        chassis_manager.get_keys(manager_model.get_uuid())) {
        response.add_entry(chassis_uuid);
    }
}


void get_acls_collections(const GetCollection::Request& request,
                          GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    const auto& switch_manager = network_components->get_switch_manager();
    auto& acl_manager = network_components->get_acl_manager();
    auto switch_module = switch_manager.get_entry(request.get_uuid());
    for (const auto& uuid : acl_manager.get_keys(switch_module.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_acl_port_collections(const GetCollection::Request& request,
                              GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    auto& acl_manager = network_components->get_acl_manager();
    auto& acl_port_manager = network_components->get_acl_port_manager();
    auto acl_model = acl_manager.get_entry(request.get_uuid());
    for (const auto& uuid : acl_port_manager.get_children(acl_model.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_port_acl_collections(const GetCollection::Request& request,
                              GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    auto& port_manager = network_components->get_port_manager();
    auto& acl_port_manager = network_components->get_acl_port_manager();
    auto port_model = port_manager.get_entry(request.get_uuid());
    for (const auto& uuid : acl_port_manager.get_parents(port_model.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_acl_rule_collections(const GetCollection::Request& request,
                              GetCollection::Response& response) {
    auto network_components = NetworkComponents::get_instance();
    auto& acl_manager = network_components->get_acl_manager();
    auto& acl_rule_manager = network_components->get_acl_rule_manager();
    auto acl_model = acl_manager.get_entry(request.get_uuid());
    for (const auto& uuid : acl_rule_manager.get_keys(acl_model.get_uuid())) {
        response.add_entry(uuid);
    }
}


void get_static_macs_collection(const GetCollection::Request& request,
                                GetCollection::Response& response) {
    auto network_manager = NetworkComponents::get_instance();
    auto& static_mac_manager = network_manager->get_static_mac_manager();
    auto& port_manager = network_manager->get_port_manager();
    //verify port UUID
    const std::string& port_uuid = request.get_uuid();
    port_manager.get_entry(port_uuid);
    for (const auto& uuid : static_mac_manager.get_keys(port_uuid)) {
        response.add_entry(uuid);
    }
}


void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
    CollectionName vlans{CollectionName::Vlans};
    CollectionName members{CollectionName::PortMembers};
    CollectionName switches{CollectionName::Switches};
    CollectionName ports{CollectionName::EthernetSwitchPorts};
    CollectionName neighbors{CollectionName::Neighbors};
    CollectionName chassis{CollectionName::Chassis};
    CollectionName acls{CollectionName::Acls};
    CollectionName port_acls{CollectionName::PortAcls};
    CollectionName acl_ports{CollectionName::AclPorts};
    CollectionName acl_rules{CollectionName::AclRules};
    CollectionName static_macs{CollectionName::StaticMacs};

    if (vlans.to_string() == request.get_name()) {
        get_vlan_collections(request, response);
    }
    else if (members.to_string() == request.get_name()) {
        get_member_collections(request, response);
    }
    else if (switches.to_string() == request.get_name()) {
        get_switch_collections(request, response);
    }
    else if (ports.to_string() == request.get_name()) {
        get_port_collections(request, response);
    }
    else if (neighbors.to_string() == request.get_name()) {
        get_neighbor_collections(request, response);
    }
    else if (chassis.to_string() == request.get_name()) {
        get_chassis_collections(request, response);
    }
    else if (port_acls.to_string() == request.get_name()) {
        get_port_acl_collections(request, response);
    }
    else if (acls.to_string() == request.get_name()) {
        get_acls_collections(request, response);
    }
    else if (acl_ports.to_string() == request.get_name()) {
        get_acl_port_collections(request, response);
    }
    else if (acl_rules.to_string() == request.get_name()) {
        get_acl_rule_collections(request, response);
    }
    else if (static_macs.to_string() == request.get_name()) {
        get_static_macs_collection(request, response);
    }
    else {
        THROW(agent_framework::exceptions::InvalidCollection, "network-agent",
              "Unknown collection requested " + request.get_name());
    }
    log_debug("network-agent", "Getting collection \"" + request.get_name() + "\" is successfully finished.");
}
}

REGISTER_COMMAND(GetCollection, ::get_collection);



