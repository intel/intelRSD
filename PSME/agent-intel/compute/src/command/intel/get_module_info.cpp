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

#include "agent-framework/command/compute/get_module_info.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::command;

/*! GetModuleInfo implementation */
class GetModuleInfo : public compute::GetModuleInfo {
public:
    using compute::GetModuleInfo::execute;

    void execute(const Request& request, Response& response) {
        auto module = ComputeComponents::get_instance()->
                get_chassis_model().get_entry(
                                        request.get_module());
        module.set_status(agent_framework::model::attribute::
                            Status(module.get_present()));
        process_collections(module, request.get_module());
        response.set_module(std::move(module));
    }

    ~GetModuleInfo();

private:
    void process_collections(ComputeModule& module, const std::string& name) {
        if (ComputeComponents::get_instance()->
                get_system_manager().get_entry_count(name)) {
            module.add_collection({enums::CollectionName::Blades, enums::CollectionType::Blades});
        }
    }
};

GetModuleInfo::~GetModuleInfo() { }

static Command::Register<GetModuleInfo> g("Intel");
