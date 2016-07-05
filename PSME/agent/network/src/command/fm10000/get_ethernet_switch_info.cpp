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

#include "agent-framework/command/network/get_ethernet_switch_info.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/dcrp/remote_switch_info.hpp"
#include "loader/network_config.hpp"
#include "network_config.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent::network::api::netlink;
using namespace agent::network::api::dcrp;
using namespace agent::network::loader;

namespace {
    string get_unique_switch_identifier() {
        string ret{};
        try {
            ret = RemoteSwitchInfo::get_local_node_id();
        }
        catch (std::runtime_error& e) {
            log_error(GET_LOGGER("fm10000"),
                      string("Unable to read local node id from DCRP: ")
                      + e.what());
        }
        return ret;
    }
}

namespace fm10000 {

/*! Network command GetEthernetSwitchInfo */
class GetEthernetSwitchInfo final : public network::GetEthernetSwitchInfo {

public:
    /*! Command constructor */
    GetEthernetSwitchInfo() { }

    using network::GetEthernetSwitchInfo::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        const auto& switch_manager = network_manager->get_switch_manager();
        auto switch_module = switch_manager.get_entry(request.get_switch());

        const auto& mgmt_port = NetworkConfig().get_instance()->
                get_switch_mgmt_port(switch_module.get_switch_identifier());
        SwitchInfo switch_info{mgmt_port};
        if (switch_info.get_is_enabled()) {
            switch_module.set_status({enums::State::Enabled,
                                    enums::Health::OK});
            /* read switch information thru Netlink */
            switch_info.read_switch_info();
            switch_module.set_mac_address(switch_info.get_mac_address());
        }
        else {
            switch_module.set_status({enums::State::Disabled,
                                    enums::Health::Warning});
        }

        // get unique switch identifier from DCRP API
        switch_module.set_switch_identifier(::get_unique_switch_identifier());
        response.set_switch(std::move(switch_module));
    }

    /*! Command destructor */
    ~GetEthernetSwitchInfo();
};

GetEthernetSwitchInfo::~GetEthernetSwitchInfo() { }

}

static Command::Register<fm10000::GetEthernetSwitchInfo> g("fm10000");
