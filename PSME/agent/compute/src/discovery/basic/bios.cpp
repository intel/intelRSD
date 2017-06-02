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
 *
 * @file bios.cpp
 *
 * @brief ipmi-way of discovering bios properties
 * */

#include "discovery/common.hpp"
#include "discovery/basic/bios.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"

using namespace agent_framework::model;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, enums::Health> discover_bios(ipmi::ManagementController& mc, System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetBiosVersion.");
    sdv::request::GetBiosVersion request{};
    sdv::response::GetBiosVersion response{};

    try {
        mc.send(request, response);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetBiosVersion: "
                                       << e.what()
                                       << " for System: "
                                       << system.get_uuid());
        return std::make_tuple(false, enums::Health::Warning);
    }

    auto cc = response.get_completion_code();

    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_bad_completion_code(system.get_uuid(), cc, "GetBiosVersion");
        return std::make_tuple(false, enums::Health::Warning);
    }
    system.set_bios_version(response.get_version());

    log_debug(GET_LOGGER("basic-discovery"), "GetBiosVersion successful.");
    log_info(GET_LOGGER("basic-discovery"), "BIOS Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
