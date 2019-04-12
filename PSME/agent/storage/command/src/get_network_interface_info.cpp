/*!
 * @brief Implementation of getNetworkInterfaceInfo command.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file get_network_interface_info.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "storage_agent_commands.hpp"


using namespace agent_framework;
using namespace agent::storage;

REGISTER_STORAGE_COMMAND(GetNetworkInterfaceInfo,
                         [](GetNetworkInterfaceInfo::ContextPtr,
                            const GetNetworkInterfaceInfo::Request& request,
                            GetNetworkInterfaceInfo::Response& response) {
    log_debug("storage-agent", "Getting network interface info.");
    response = module::get_manager<model::NetworkInterface>().get_entry(request.get_uuid());
});
