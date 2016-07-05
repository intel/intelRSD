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

#include "agent-framework/command/network/get_manager_info.hpp"
#include "agent-framework/module-ref/network_manager.hpp"

using namespace agent_framework::module;
using namespace agent_framework::command;

namespace fm10000 {

class GetManagerInfo : public network::GetManagerInfo {
public:
    GetManagerInfo() { }

    using network::GetManagerInfo::execute;

    void execute(const Request& request, Response& response) {
        response.set_manager(NetworkManager::get_instance()->
                                    get_module_manager().
                                    get_entry(request.get_manager()));
    }

    ~GetManagerInfo();
};

GetManagerInfo::~GetManagerInfo() { }

}

static Command::Register<fm10000::GetManagerInfo> g("fm10000");
