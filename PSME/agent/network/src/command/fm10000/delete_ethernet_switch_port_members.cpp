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

#include "agent-framework/command/network/delete_ethernet_switch_port_members.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/logger_loader.hpp"

#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "network_config.hpp"

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent::network;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;

namespace fm10000 {

/*! Network command DeleteEthernetSwitchPortMembers */
class DeleteEthernetSwitchPortMembers final : public command::network::DeleteEthernetSwitchPortMembers {
public:
    /*! Command constructor */
    DeleteEthernetSwitchPortMembers() { }

    using agent_framework::command::network::DeleteEthernetSwitchPortMembers::execute;

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
     * @brief Check if member ports are present
     *
     * @param[in] members Port members
     * */
    void check_if_members_are_present(const vector<string>& members) {
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();

        /* check that member ports are present */
        for (const auto& member_uuid : members) {
            if (!port_manager.entry_exists(member_uuid)) {
                THROW(agent_framework::exceptions::InvalidUuid, "fm10000",
                      std::string("Member port not found ") + member_uuid);
            }
        }
    }

    /*!
     * @brief Check member size
     *
     * @param[in] port Port model (LAG)
     * @param[in] members Port members
     * */
    void check_members_size(const SwitchPort& port, const vector<string>& members) {
        auto controller = NetworkControllerManager::get_network_controller();

        auto port_members =
            controller->get_logical_port_members(port.get_port_identifier());

        /* According to the requirements, empty Lag is not
         * supported. Execution of DeleteSwitchPort will cause
         * removing of Lag and member ports */
        if (port_members.size() <= members.size()) {
            THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
                  "Removing all members from Lag is not permitted");
        }
    }

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * */
    void execute(const Request& request, Response&) {
        auto controller = NetworkControllerManager::get_network_controller();
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto port = port_manager.get_entry(request.get_port());
        const auto& members = request.get_members();
        SwitchPortInfo port_info(port.get_port_identifier());

        /* Port must be a logical port */
        check_if_logical(port);

        check_if_members_are_present(members);

        check_members_size(port, members);

        /* remove port members from the LAG port */
        for (const auto& member_uuid : members) {
            const auto& member_port = port_manager.get_entry(member_uuid);

            /* remove member from the LAG port */
            SwitchPortInfo::remove_member(member_port.get_port_identifier());

            /* update VLAN UUID on port */
            controller->init_switch_vlan_port(member_port.get_port_identifier());
        }
    }

    /*! Command destructor */
    ~DeleteEthernetSwitchPortMembers();
};

DeleteEthernetSwitchPortMembers::~DeleteEthernetSwitchPortMembers() { }

} /* fm10000 */

static Command::Register<fm10000::DeleteEthernetSwitchPortMembers> g("fm10000");
