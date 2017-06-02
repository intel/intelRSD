/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "discovery/common.hpp"
#include "discovery/basic/system.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/generic/get_system_guid.hpp"
#include "agent-framework/module/compute_components.hpp"

using namespace std;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::compute::discovery;
using namespace agent::compute::discovery::basic;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, agent_framework::model::enums::Health> discover_system(ipmi::ManagementController& mc, System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetSystemGuid.");
    generic::request::GetSystemGuid request{};
    generic::response::GetSystemGuid response{};

    try {
        mc.send(request, response);
    }
    catch (const runtime_error& e) {
        log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetSystemGuid: "
                                       << e.what()
                                       << " for System: "
                                       << system.get_uuid());
        return make_tuple(false, enums::Health::Warning);
    }

    const auto cc = response.get_completion_code();

    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_bad_completion_code(system.get_uuid(), cc, "GetSystemGuid");
        return make_tuple(false, enums::Health::Warning);
    }

    system.set_guid(response.get_guid());
    log_debug(GET_LOGGER("basic-discovery"), "System: " << system.get_uuid() << " GUID is " << system.get_guid());
    log_debug(GET_LOGGER("basic-discovery"), "GetSystemGuid successful.");

    log_info(GET_LOGGER("basic-discovery"), "System Discovery was successful.");
    return make_tuple(true, enums::Health::OK);
}

}
}
}
}
