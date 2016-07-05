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

#include "agent-framework/command/network/add_ethernet_switch_port.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/netlink/add_lag_port_message.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::command;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;

namespace fm10000 {

/*! Network command AddEthernetSwitchPort */
class AddEthernetSwitchPort final : public network::AddEthernetSwitchPort {
public:
    /*! Command constructor */
    AddEthernetSwitchPort() { }

    using network::AddEthernetSwitchPort::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto controller = NetworkControllerManager::get_network_controller();
        const auto& port = request.get_port_identifier();
        const auto& mode = request.get_mode();
        const auto& members = request.get_members();
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto& switch_manager = network_manager->get_switch_manager();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        auto switch_model = switch_manager.get_entry(request.get_switch());

        /* Limitation of the BDC-R Switch - max lag length name = 15. */
        if (MAX_PORTID_LENGTH < port.length()) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                string("Port ID too long: max length is ") +
                std::to_string(MAX_PORTID_LENGTH));
        }

        /* According to the requirements, empty Lag is not supported.
         * At least one member should be specified */
        if (members.empty()) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                "No members specified");
        }

        /* RRC can handle up to 16 ports in a single LAG */
        if (members.size() > MAX_LAG_MEMBER_COUNT) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                  string("Maximum LAG members count is ") +
                  std::to_string(MAX_LAG_MEMBER_COUNT));
        }

        /* validate port mode */
        if (PortMode::LinkAggregationStatic != mode) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                string("Invalid port mode: ") + string(mode.to_string()));
        }

        /* validate port members */
        controller->validate_port_members(members);

        /* add specified port to the switch thru Netlink API */
        SwitchInfo switch_info{{}};
        switch_info.add_switch_port(port,
                            SwitchInfo::PortMode::LINK_AGGREGATION_STATIC);
        auto lag_uuid = controller->add_logical_port(port,
                                                    switch_model.get_uuid());

        /* add port members into the LAG port */
        for (const auto& member_uuid : members) {
            const auto member_port = port_manager.get_entry(member_uuid);
            SwitchPortInfo port_info(member_port.get_port_identifier());

            /* force member port admin state "Down" */
            SwitchPortInfo::PortAttributeValue value{};
            value.set(string(enums::AdministrativeState(
                                enums::AdministrativeState::Down).to_string()));
            port_info.set_switch_port_attribute(
                                SwitchPortInfo::ADMINISTRATIVESTATE, value);

            /* add member into the LAG port */
            port_info.add_member(port, member_port.get_port_identifier());

            /* remove vlan UUIDs from the member port */
            port_vlan_manager.remove_by_parent(member_port.get_uuid());
        }

        /* update VLAN on the LAG */
        controller->init_switch_vlan_port(port);

        response.set_port(lag_uuid);
    }

    /*! Command destructor */
    ~AddEthernetSwitchPort();

private:
    static const int MAX_PORTID_LENGTH = 15;

};

AddEthernetSwitchPort::~AddEthernetSwitchPort() { }

}

static Command::Register<fm10000::AddEthernetSwitchPort> g("fm10000");
