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
 * @section DESCRIPTION
 * */
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/command/compute/get_manager_collection.hpp"

using namespace agent_framework::command;

/*! Stubs GetManagerCollection implementation */
class GetManagerCollection : public compute::GetManagerCollection {
public:

    using ComputeComponents = agent_framework::module::ComputeManager;

    GetManagerCollection() {}

    using compute::GetManagerCollection::execute;

    void execute(const Request&, Response& response) {
        auto keys = ComputeComponents::get_instance()->
                get_module_manager().get_keys();
        for (auto key : keys) {
            response.add_manager(
                agent_framework::model::attribute::ManagerEntry(key));
        }
    }

    ~GetManagerCollection();
};

GetManagerCollection::~GetManagerCollection() {}

static Command::Register<GetManagerCollection> g("Stubs");

