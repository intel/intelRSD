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

#include "agent-framework/command/network/delete_ethernet_switch_port.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "api/netlink/switch_info.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::command;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;
using namespace agent_framework::exceptions;

namespace fm10000 {

/*! Network command DeleteEthernetSwitchPort */
class DeleteEthernetSwitchPort final : public agent_framework::command::network::DeleteEthernetSwitchPort {
public:
    /*! Command constructor */
    DeleteEthernetSwitchPort() { }

    using agent_framework::command::network::DeleteEthernetSwitchPort::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * */
    void execute(const Request& request, Response&) {
        auto controller = NetworkControllerManager::get_network_controller();
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        const auto port = port_manager.get_entry(request.get_port());

        /* only LAG port can be removed */
        if (PortClass::Logical != port.get_port_class()) {
            THROW(InvalidParameters, "fm10000", "Physical port can't be removed");
        }

        /* get LAG members */
        auto members = controller->get_logical_port_members(
                                                port.get_port_identifier());

        /* delete the port thru Netlink API */
        SwitchInfo info{{}};
        info.delete_switch_port(port.get_port_identifier());

        /* update VLAN UUID on members of the Lag */
        for (const auto& member : members) {
            controller->init_switch_vlan_port(member);
        }

        /* remove VLAN UUID on the LAG */
        port_vlan_manager.remove_by_parent(port.get_uuid());

        /* delete the port form network manager */
        port_manager.remove_entry(port.get_uuid());
    }

    /*! Command destructor */
    ~DeleteEthernetSwitchPort();
};

DeleteEthernetSwitchPort::~DeleteEthernetSwitchPort() { }

}

static Command::Register<fm10000::DeleteEthernetSwitchPort> g("fm10000");

