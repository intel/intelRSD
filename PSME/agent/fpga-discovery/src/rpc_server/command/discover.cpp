/*!
 * @brief Implementation of discover command.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file discover.cpp
 */

#include "rpc_server/command/discover.hpp"
#include "tools/discovery_entries_provider.hpp"
#include "agent-framework/module/common_components.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace agent::fpga_discovery;
using namespace agent_framework::model;
using namespace agent_framework::command;

namespace agent {
namespace fpga_discovery {
namespace command {

using agent::fpga_discovery::command::Discover;

void discover(Discover::ContextPtr /*ctx*/,
              const Discover::Request& request,
              Discover::Response& response) {

    agent::fpga_discovery::DiscoveryEntriesProvider provider{};
    auto entries = provider.get_discovery_entries(request.get_host_id());
    response.set_discovery_entries(entries);

    log_info("fpga-discovery-agent", " discover req: " << request.to_json().dump()
             << "\n resp: " << response.to_json().dump());
}

}
}
}
