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
#include "agent-framework/logger_ext.hpp"
#include <sstream>

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::generic;

/*! Network command AddEthernetSwitchPort */
class AddEthernetSwitchPort final : public command::network::AddEthernetSwitchPort {
public:
    /*! Command constructor */
    AddEthernetSwitchPort() { }

    using agent_framework::command::network::AddEthernetSwitchPort::execute;

    /*


     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    void execute(const Request&, Response&) {
        std::stringstream message{};
        log_info(GET_LOGGER("network-agent"), std::string("Stub command: \
            addEthernetSwitchPort not supported yet"));
        /*message << "Adding new logical port to switch " << request.get_switch();
        log_info(GET_LOGGER("network-agent"), message.str());

        message.str({});
        message << "\n\tPort Identifier: " << request.get_port_identifier()
                << "\n\tMode: " << request.get_mode().to_string()
                << "\n\tMembers: ";

        const auto& members = request.get_members();
        for (const auto& member_uuid : members) {
            message << "\n\t\t" << member_uuid;
        }
        log_info(GET_LOGGER("network-agent"), message.str());

        response.set_port("12345678-e89b-12d3-a451-e36655440411");
        response.set_oem({});*/
    }

    /*! Command destructor */
    ~AddEthernetSwitchPort();
};

AddEthernetSwitchPort::~AddEthernetSwitchPort() { }

static Command::Register<AddEthernetSwitchPort> g("Stubs");
