/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * */

#include "spdk_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::spdk;

namespace {

void get_managers_collection(GetManagersCollection::ContextPtr,
                             const GetManagersCollection::Request&,
                             GetManagersCollection::Response& response) {

    for (const auto& uuid : get_manager<Manager>().get_keys()) {
        response.add_entry(attribute::ManagerEntry(uuid));
    }
    log_debug("spdk-agent", "Getting managers collection is successfully finished.");
}
}

REGISTER_SPDK_COMMAND(GetManagersCollection, ::get_managers_collection);
