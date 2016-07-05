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

#include "agent-framework/command/compute/get_dimm_info.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;

class GetDimmInfo : public compute::GetDimmInfo {
public:
    using agent_framework::command::compute::GetDimmInfo::execute;
    using ComputeComponents = agent_framework::module::ComputeManager;

    void execute(const Request& request, Response& response) {
        response.set_dimm(ComputeComponents::get_instance()->
                                    get_dimm_manager().get_entry(
                                            request.get_dimm()));
    }

    ~GetDimmInfo();
};

GetDimmInfo::~GetDimmInfo() { }

static Command::Register<GetDimmInfo> g("Stubs");
