/*!
 * @brief Get Processor Info command
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_processor_info.cpp
 */


#include "agent-framework/module/common_components.hpp"
#include "fpgaof_agent_commands.hpp"



using namespace agent_framework;
using namespace agent::fpgaof;

namespace {

void get_processor_info(GetProcessorInfo::ContextPtr,
                        const GetProcessorInfo::Request& request,
                        GetProcessorInfo::Response& response) {
    log_debug("fpgaof-agent", "Getting processor info.");
    response = module::get_manager<model::Processor>().get_entry(request.get_uuid());
}

}

REGISTER_FPGAOF_COMMAND(GetProcessorInfo, ::get_processor_info);
