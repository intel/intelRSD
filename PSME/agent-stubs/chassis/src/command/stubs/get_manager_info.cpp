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
 *
 * @file command/stubs/get_manager_info.cpp
 * @brief GetManagerInfo stub implementation
 * */

#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/command/chassis/get_manager_info.hpp"

using namespace agent_framework::module;
using namespace agent_framework::command;

class GetManagerInfo : public chassis::GetManagerInfo {
public:

    using ChassisComponents = agent_framework::module::ChassisManager;

    GetManagerInfo() {}

    using chassis::GetManagerInfo::execute;

    void execute(const Request& request, Response& response) {
        log_debug(GET_LOGGER("rpc"), "GetManagerInfo with parameters: manager " << request.get_component());

        response.set_manager(ChassisComponents::get_instance()->
                get_module_manager().get_entry(
                request.get_component()));
    }

    ~GetManagerInfo();
};

GetManagerInfo::~GetManagerInfo() { }

static Command::Register<GetManagerInfo> g("Stubs");
