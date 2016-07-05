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

#include "agent-framework/command/network/delete_port_vlan.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::command;
using namespace agent_framework::generic;

/*! Network command DeletePortVlan */
class DeletePortVlan final :
    public network::DeletePortVlan {
public:
    /*! Command constructor */
    DeletePortVlan() { }

    using network::DeletePortVlan::execute;
    using NetworkManager = agent_framework::module::NetworkManager;

    /*
     * @brief Execute command with given request and response argument
     *
     * @param[out]  response Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto& uuid = request.get_port_vlan();
        const auto& oem = request.get_oem();
        std::string message{};

        if(NetworkManager::get_instance()->
            get_port_vlan_manager().entry_exists(uuid)) {
            NetworkManager::get_instance()->
            get_port_vlan_manager().remove_entry(uuid);
            response.set_oem(oem);
            message = "Deleted port vlan with uuid: " + uuid;
            log_info(GET_LOGGER("network-agent"), message);
        }
        else {
            message = "No port vlan with uuid: " + uuid;
            THROW(::agent_framework::exceptions::InvalidUuid,
                    "network-agent", message);
        }
    }

    /*! Command destructor */
    ~DeletePortVlan();
};

DeletePortVlan::~DeletePortVlan() { }

static Command::Register<DeletePortVlan> g("Stubs");
