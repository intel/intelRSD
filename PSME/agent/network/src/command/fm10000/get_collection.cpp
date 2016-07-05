/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 * */

#include "agent-framework/command/network/get_collection.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/module-ref/types/common.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/logger_ext.hpp"
#include "network_config.hpp"

#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"

#include "api/remote_switch_info.hpp"
#include "api/dcrp/remote_switch_info.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model::literals;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::types;
using namespace agent::network::hw::fm10000;
using namespace agent::network::api::dcrp;

namespace fm10000 {

using namespace agent_framework::model::types;

/*! GetCollection implementation */
class GetCollection : public network::GetCollection {
public:
    GetCollection() {}

    using network::GetCollection::execute;

    void execute(const Request& request, Response& response) {
        CollectionName vlans{CollectionName::Vlans};
        CollectionName members{CollectionName::PortMembers};
        CollectionName switches{CollectionName::Switches};
        CollectionName ports{CollectionName::Ports};
        CollectionName port_acls{CollectionName::PortAcls};
        CollectionName neighbors{CollectionName::Neighbors};
        CollectionName chassis{CollectionName::Chassis};
        CollectionName acls{CollectionName::Acls};
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
        else if (port_acls.to_string() == request.get_name()) {
            get_port_acl_collections(request, response);
        }
        else if (neighbors.to_string() == request.get_name()) {
            get_neighbor_collections(request, response);
        }
        else if (chassis.to_string() == request.get_name()) {
            get_chassis_collections(request, response);
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
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                std::string("Unknown collection requested ") +
                std::string(request.get_name()));
        }
        log_debug(GET_LOGGER("fm10000"), "Getting collection is successfully finished.");
    }

    ~GetCollection();

private:
    void get_member_collections(const Request& request, Response& response) {
        auto controller = NetworkControllerManager::get_network_controller();
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto port = port_manager.get_entry(request.get_component());
        if (PortClass::Logical != port.get_port_class()) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                std::string("Port is not LAG [uuid=") + port.get_uuid() + "]");
        }
        for (const auto& member_uuid : port_manager.get_keys()) {
            auto member = port_manager.get_entry(member_uuid);
            if (controller->is_member_port(member.get_port_identifier())) {
                const auto lag_identifier = controller->
                            get_lag_identifier(member.get_port_identifier());
                if (port.get_port_identifier() == lag_identifier) {
                    response.add_subcomponent(member_uuid);
                }
            }
        }
    }

    void get_vlan_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        const auto& port_manager = network_manager->get_port_manager();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        auto port = port_manager.get_entry(request.get_component());
        for (const auto& uuid : port_vlan_manager.get_keys(port.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_port_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        const auto& switch_manager = network_manager->get_switch_manager();
        auto& port_manager = network_manager->get_port_manager();
        auto switch_module = switch_manager.get_entry(request.get_component());
        for (const auto& uuid : port_manager.get_keys(switch_module.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_switch_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& manager = network_manager->get_module_manager();
        auto& switch_manager = network_manager->get_switch_manager();
        auto manager_model = manager.get_entry(request.get_component());
        for (const auto& switch_uuid :
            switch_manager.get_keys(manager_model.get_uuid())) {
                response.add_subcomponent(switch_uuid);
        }
    }

    void get_neighbor_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& remote_switch_manager = network_manager->get_remote_switch_manager();
        try {
            RemoteSwitchInfo info;
            // get neighbors of a local switch by giving an empty switch id
            info.load_neighbor_switches_id(request.get_component());
            for (const auto& uuid : remote_switch_manager.get_keys()) {
                response.add_subcomponent(uuid);
            }
        }
        catch (std::runtime_error& err) {
            /* DCRP is not enabled, continue */
            log_error(GET_LOGGER("fm10000"), "Getting neighbor switches failed");
            log_debug(GET_LOGGER("fm10000"), err.what());
        }
    }

    void get_chassis_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& manager = network_manager->get_module_manager();
        auto& chassis_manager = network_manager->get_chassis_manager();
        auto manager_model = manager.get_entry(request.get_component());
        for (const auto& chassis_uuid :
            chassis_manager.get_keys(manager_model.get_uuid())) {
                response.add_subcomponent(chassis_uuid);
        }
    }

    void get_acls_collections(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        const auto& switch_manager = network_manager->get_switch_manager();
        auto& acl_manager = network_manager->get_acl_manager();
        auto switch_module = switch_manager.get_entry(request.get_component());
        for (const auto& uuid : acl_manager.get_keys(switch_module.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_acl_port_collections(const Request& request, Response& response) {
        auto network_components = NetworkManager::get_instance();
        auto& acl_manager = network_components->get_acl_manager();
        auto& acl_port_manager = network_components->get_acl_port_manager();
        auto acl_model = acl_manager.get_entry(request.get_component());
        for (const auto& uuid : acl_port_manager.get_children(acl_model.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_port_acl_collections(const Request& request, Response& response) {
        auto network_components = NetworkManager::get_instance();
        auto& port_manager = network_components->get_port_manager();
        auto& acl_port_manager = network_components->get_acl_port_manager();
        auto port_model = port_manager.get_entry(request.get_component());
        for (const auto& uuid : acl_port_manager.get_parents(port_model.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_acl_rule_collections(const Request& request, Response& response) {
        auto network_components = NetworkManager::get_instance();
        auto& acl_manager = network_components->get_acl_manager();
        auto& acl_rule_manager = network_components->get_acl_rule_manager();
        auto acl_model = acl_manager.get_entry(request.get_component());
        for (const auto& uuid : acl_rule_manager.get_keys(acl_model.get_uuid())) {
            response.add_subcomponent(uuid);
        }
    }

    void get_static_macs_collection(const Request& request,
                                    Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& static_mac_manager = network_manager->get_static_mac_manager();
        auto& port_manager = network_manager->get_port_manager();
        const string& port_uuid = request.get_component();
        //verify port UUID
        port_manager.get_entry(port_uuid);
        for (const auto& uuid : static_mac_manager.get_keys(port_uuid)) {
            response.add_subcomponent(uuid);
        }
    }
};

GetCollection::~GetCollection() {}

}

static Command::Register<fm10000::GetCollection> g("fm10000");

