/*!
 * @brief Implementation of GetManagersCollection command.
 *
 * File contains all implementations of methods for GetManagersCollection command.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_managers_collection.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace agent::nvme;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {
void get_managers_collection(GetManagersCollection::ContextPtr, const GetManagersCollection::Request&,
        GetManagersCollection::Response& response) {
    for (const auto& uuid : get_manager<Manager>().get_keys()) {
        response.add_entry(attribute::ManagerEntry(uuid));
    }
    log_debug("nvme-agent", "Getting managers collection is successfully finished.");
}
}

REGISTER_NVME_COMMAND(GetManagersCollection, ::get_managers_collection);
