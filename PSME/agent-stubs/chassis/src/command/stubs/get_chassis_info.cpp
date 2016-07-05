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
 * @file command/stubs/get_chassis_info.cpp
 * @brief GetChassisInfo stub implementation
 * */

#include "agent-framework/command/chassis/get_chassis_info.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::command;

class GetChassisInfo : public agent_framework::command::chassis::GetChassisInfo {
public:
    using ChassisComponents = agent_framework::module::ChassisManager;

    GetChassisInfo() {}

    using chassis::GetChassisInfo::execute;

    void execute(const Request& request, Response& response) {
        log_debug(GET_LOGGER("rpc"), "GetChassisInfo with parameters: chassis "
             << request.get_chassis());
        response.set_chassis(ChassisManager::get_instance()->
                get_chassis_manager().get_entry(request.get_chassis()));
    }

    ~GetChassisInfo();
};

GetChassisInfo::~GetChassisInfo() { }

static Command::Register<GetChassisInfo> g("Stubs");
