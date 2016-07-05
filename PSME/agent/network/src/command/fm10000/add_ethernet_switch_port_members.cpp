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

#include "agent-framework/command/network/add_ethernet_switch_port_members.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/enum/network.hpp"

#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "agent-framework/logger_loader.hpp"
#include "network_config.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/netlink/add_lag_port_message.hpp"
#include "api/switch_port_info.hpp"


using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent::network;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;

namespace fm10000 {

/*! Network command AddEthernetSwitchPortMembers */
class AddEthernetSwitchPortMembers final : public command::network::AddEthernetSwitchPortMembers {
public:
    /*! Command constructor */
    AddEthernetSwitchPortMembers() { }

    using agent_framework::command::network::AddEthernetSwitchPortMembers::execute;

    /*!
     * @brief Check if port is logical
     *
     * @param[in] port Port model
     * */
    void check_if_logical(const SwitchPort& port) {
        if (enums::PortClass::Logical != port.get_port_class()) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                  std::string("Port is not LAG [uuid=") + port.get_uuid() + "]");
        }
    }

    /*!
     * @brief Validate port members
     *
     * @param[in] port Port model (LAG)
     * @param[in] members Port members
     * @param[out] empty true if empty otherwise false
     * */
    void validate_port_members_and_check_if_empty(const SwitchPort& port,
                                                  const std::vector<string>& members,
                                                  bool& empty) {
        auto controller = NetworkControllerManager::get_network_controller();
        auto port_members =
            controller->get_logical_port_members(port.get_port_identifier());

        if ((port_members.size() + members.size()) > MAX_LAG_MEMBER_COUNT) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                  string("Maximum LAG members count is ") +
                  std::to_string(MAX_LAG_MEMBER_COUNT));
        }

        empty = port_members.empty();
        if (!empty) {
            SwitchPortInfo::PortAttributeValue speed{};
            const auto& slaved_port = port_members.front();
            SwitchPortInfo slaved_port_info(slaved_port);
            slaved_port_info.get_switch_port_attribute(
                                        SwitchPortInfo::LINKSPEEDMBPS, speed);
            controller->validate_port_members(members, int(speed));
        } else {
            controller->validate_port_members(members);
        }
    }

    /*!
     * @brief Add member to the lag port
     *
     * @param[in] lag port model (LAG)
     * @param[in] member_port Port member
     * */
    void add_member_to_lag(const SwitchPort& lag, const SwitchPort &member_port) {
        SwitchPortInfo member_port_info(member_port.get_port_identifier());

        /* force member port admin state "Down" */
        SwitchPortInfo::PortAttributeValue value{};
        value.set(string(enums::AdministrativeState(
                                enums::AdministrativeState::Down).to_string()));
        member_port_info.set_switch_port_attribute(
                                SwitchPortInfo::ADMINISTRATIVESTATE, value);

        /* add member into the LAG port */
        SwitchPortInfo::add_member(lag.get_port_identifier(),
                                   member_port.get_port_identifier());
    }

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * */
    void execute(const Request& request, Response&) {
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        auto port = port_manager.get_entry(request.get_port());
        const auto& members = request.get_members();
        auto controller = NetworkControllerManager::get_network_controller();
        bool empty{true};

        /* Port must be a logical port */
        check_if_logical(port);

        validate_port_members_and_check_if_empty(port, members,
                                                 empty);

        /* add port members into the LAG port */
        for (const auto& member_uuid : members) {
            const auto& member_port = port_manager.get_entry(member_uuid);

            add_member_to_lag(port, member_port);

            /* remove vlan UUIDs */
            port_vlan_manager.remove_by_parent(member_port.get_uuid());
        }

        /* update VLAN on the LAG */
        if (empty) {
            controller->init_switch_vlan_port(port.get_port_identifier());
        }
    }

    /*! Command destructor */
    ~AddEthernetSwitchPortMembers();
};

AddEthernetSwitchPortMembers::~AddEthernetSwitchPortMembers() { }

} /* fm10000 */

static Command::Register<fm10000::AddEthernetSwitchPortMembers> g("fm10000");
