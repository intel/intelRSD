/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
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
 *
 * @file update_relations.cpp
 * */

#include "tree_stability/helpers/update_relations.hpp"

#include "agent-framework/module/network_components.hpp"


using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace network {
namespace helpers {

void update_acl_in_relations(const std::string& acl_temporary_uuid, const std::string& acl_persistent_uuid) {
    // ACL is a parent in ACL <-> port relation
    NetworkComponents::get_instance()->get_acl_port_manager().update_parent(acl_temporary_uuid, acl_persistent_uuid);
}


void update_port_in_relations(const std::string& port_temporary_uuid, const std::string& port_persistent_uuid) {
    // Port is a child in ACL <-> port relation
    NetworkComponents::get_instance()->get_acl_port_manager().update_child(port_temporary_uuid, port_persistent_uuid);
    // update port UUID in ACL rule attributes
    auto& acl_rule_manager = NetworkComponents::get_instance()->get_acl_rule_manager();
    for (const std::string& rule_uuid : acl_rule_manager.get_keys()) {
        auto rule = acl_rule_manager.get_entry_reference(rule_uuid);
        if (rule->get_forward_mirror_port() == port_temporary_uuid) {
            rule->set_forward_mirror_port(port_persistent_uuid);
        }
        agent_framework::model::AclRule::MirroredPorts new_mirrored_ports{};
        bool updated = false;
        for (const std::string& port_uuid : rule->get_mirrored_ports()) {
            if (port_uuid == port_temporary_uuid) {
                new_mirrored_ports.add_entry(port_persistent_uuid);
                updated = true;
            }
            else {
                new_mirrored_ports.add_entry(port_uuid);
            }
        }
        if (updated) {
            rule->set_mirrored_ports(new_mirrored_ports);
        }
    }
}

void update_chassis_in_relations(const std::string& chassis_temporary_uuid, const std::string chassis_persistent_uuid) {
    // Chassis UUID is held by Ethernet Switch
    const auto& switch_keys = get_manager<EthernetSwitch>().get_keys(
        [&chassis_temporary_uuid](const EthernetSwitch& eth_switch) {
            return eth_switch.get_chassis() == chassis_temporary_uuid;
        });
    if (switch_keys.empty()) {
        throw std::runtime_error("No switch found for chassis " + chassis_temporary_uuid);
    }

    get_manager<EthernetSwitch>().get_entry_reference(switch_keys.front())->set_chassis(chassis_persistent_uuid);

    const auto chassis_parent = get_manager<Chassis>().get_entry(chassis_persistent_uuid).get_parent_uuid();
    // Chassis UUID is the location of a manager
    get_manager<Manager>().get_entry_reference(chassis_parent)->set_location(chassis_persistent_uuid);
}

}
}
}
