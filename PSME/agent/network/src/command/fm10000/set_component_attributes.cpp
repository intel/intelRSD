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

#include "api/netlink/switch_port_info.hpp"
#include "hw/fm10000/network_controller_manager.hpp"

#include "agent-framework/command/network/set_component_attributes.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/enum/network.hpp"

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;

namespace fm10000 {

/*! SetComponentAttributes implementation */
class SetComponentAttributes : public network::SetComponentAttributes {
public:
    SetComponentAttributes() {}

    using network::SetComponentAttributes::execute;
    using PortAttributeType = SwitchPortInfo::PortAttributeType;
    using PortAttributeValue = SwitchPortInfo::PortAttributeValue;

    void check_for_empty_lag(const SwitchPort& port) {
        if (PortClass::Logical == port.get_port_class()) {
            auto controller = NetworkControllerManager::get_network_controller();
            /* it's not possible to set any of attributes on an empty LAG */
            if (controller->is_logical_port_empty(port.get_port_identifier())) {
                THROW(agent_framework::exceptions::InvalidParameters,
                      "fm10000",
                      std::string("LAG port is empty ")
                      + std::string(port.get_uuid()));
            }
        }
    }

    void set_port_attributes(const SwitchPort& port, const Attributes& attributes) {
        auto network_manager = NetworkManager::get_instance();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        SwitchPortInfo port_info(port.get_port_identifier());
        const auto& attributes_names = attributes.get_names();
        for (const auto& attribute_name : attributes_names) {
            auto type = port_info.get_port_attribute_type(attribute_name);
            PortAttributeValue value{};
            const auto& attribute_value =
                attributes.get_value(attribute_name);

            value.set_value_from_json(attribute_value);

            /* convert default vlan UUID into vlan id */
            if (PortAttributeType::DEFAULTVID == type) {
                const auto& vlan_uuid = attribute_value.asString();
                const auto& vlan_port = port_vlan_manager.get_entry(vlan_uuid);
                value.set(vlan_port.get_vlan_id());
            }
            log_debug(GET_LOGGER("fm10000"),
                      "Setting port attributes [ attribute="
                      << attribute_name << " port="
                      << port.get_port_identifier() << " ]");
            port_info.set_switch_port_attribute(type, value);
        }
    }

    void execute(const Request& request, Response&) {
        const auto& uuid = request.get_component();
        const auto& attributes = request.get_attributes();
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        auto& rule_manager = network_manager->get_acl_rule_manager();
        auto& static_mac_manager = network_manager->get_static_mac_manager();

        /* check if this is switch port attributes */
        if (port_manager.entry_exists(uuid)) {
            auto port = port_manager.get_entry(uuid);

            /* make sure port is not an empty LAG */
            check_for_empty_lag(port);

            set_port_attributes(port, attributes);

            return;
        }

        /* check if this is VLAN component */
        if (port_vlan_manager.entry_exists(uuid)) {
            const auto& vlan_port = port_vlan_manager.get_entry(uuid);
            log_debug(GET_LOGGER("fm10000"),
                      "Setting vlan port attributes isn't supported [ vlan="
                      << vlan_port.get_vlan_id() << " uuid="
                      << uuid << " ]");
            THROW(agent_framework::exceptions::UnsupportedParameter,
                  "fm10000",
                  "Operation not supported "
                  "on FM10000 device for VLAN ports");
        }

        /* check if this is ACL rule */
        if (rule_manager.entry_exists(uuid)) {
            log_debug(GET_LOGGER("fm10000"),
                      "Setting ACL rule attributes isn't supported [ uuid="
                      << uuid << " ]");
            THROW(agent_framework::exceptions::UnsupportedParameter,
                  "fm10000",
                  "Operation not supported "
                  "on FM10000 device for ACL rules");
        }

        /* check if this is Static MAC */
        if (static_mac_manager.entry_exists(uuid)) {
            log_debug(GET_LOGGER("fm10000"),
                      "Setting Static MAC attributes isn't supported [ uuid="
                      << uuid << " ]");
            THROW(agent_framework::exceptions::UnsupportedParameter,
                  "fm10000",
                  "Operation not supported "
                  "on FM10000 device for Static MACs");
        }

        /* no component found */
        log_warning(GET_LOGGER("fm10000"), "Component uuid=" << uuid << " not found.");
        THROW(agent_framework::exceptions::InvalidUuid, "fm10000",
              std::string("Component UUID not found ") + std::string(uuid));
    }

    ~SetComponentAttributes();
};

SetComponentAttributes::~SetComponentAttributes() {}

}

static Command::Register<fm10000::SetComponentAttributes> g("fm10000");
