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

#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/command/network/get_manager_info.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

/*! Stubs GetManagerInfo implementation */
class GetManagerInfo : public agent_framework::command::network::GetManagerInfo {
public:
    GetManagerInfo() { }

    using NetworkManager = agent_framework::module::NetworkManager;

    using agent_framework::command::network::GetManagerInfo::execute;

    void execute(const Request& request, Response& response) {

        auto& module_manager = NetworkManager::get_instance()->
            get_module_manager();
        std::string uuid = request.get_manager();
        Manager manager{};

        if (module_manager.entry_exists(uuid)) {
            manager = module_manager.get_entry(uuid);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid,
                "network-agent", "Manager not found, uuid " + uuid);
        }
        response.set_manager(std::move(manager));
    }

    ~GetManagerInfo();
};

GetManagerInfo::~GetManagerInfo() { }

static Command::Register<GetManagerInfo> g("Stubs");
