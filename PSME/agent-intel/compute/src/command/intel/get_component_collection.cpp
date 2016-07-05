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

#include "agent-framework/command/compute/get_component_collection.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/module-ref/constants/components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::command;

/*! GetComponentCollection implementation */
class GetComponentCollection : public compute::GetComponentCollection {
public:
    using compute::GetComponentCollection::execute;

    void execute(const Request&, Response& response) {
        for (const auto& chassis : ChassisManager::get_instance()->
                chassis().get_keys()) {
                response.add_component({chassis, components::CHASSIS});
        }
        for (const auto& compute : ComputeComponents::get_instance()->
                get_chassis_model().get_keys()) {
            if (ComputeComponents::get_instance()->
                    get_chassis_model().get_present(compute)) {
                response.add_component({compute, components::COMPUTE_MODULE});
            }
        }

        log_debug(GET_LOGGER("rpc"),
            "Getting component collection is successfully finished.");
    }

    ~GetComponentCollection();
};

GetComponentCollection::~GetComponentCollection() {}

static Command::Register<GetComponentCollection> g("Intel");
