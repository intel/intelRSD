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

#include "agent-framework/command/network/get_manager_collection.hpp"
#include "agent-framework/module-ref/network_manager.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::command;

namespace fm10000 {

/*! GetManagerCollection implementation */
class GetManagerCollection : public network::GetManagerCollection {
public:
    GetManagerCollection() {}

    using network::GetManagerCollection::execute;

    void execute(const Request&, Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& manager = network_manager->get_module_manager();
        for (const auto& uuid : manager.get_keys()) {
            response.add_manager(attribute::ManagerEntry(uuid));
        }
        log_debug(GET_LOGGER("fm10000"), "Getting manager collection is successfully finished.");
    }

    ~GetManagerCollection();
};

GetManagerCollection::~GetManagerCollection() {}

}

static Command::Register<fm10000::GetManagerCollection> g("fm10000");

