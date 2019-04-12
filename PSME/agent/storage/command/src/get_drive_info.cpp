/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "agent-framework/module/common_components.hpp"
#include "storage_agent_commands.hpp"

using namespace agent_framework::module;
using namespace agent::storage;

REGISTER_STORAGE_COMMAND(GetDriveInfo,
    [] (GetDriveInfo::ContextPtr, const GetDriveInfo::Request& request, GetDriveInfo::Response& response) {
        log_debug("storage-agent", "Getting drive info.");
        response = get_manager<agent_framework::model::Drive>().get_entry(request.get_uuid());
    }
);
