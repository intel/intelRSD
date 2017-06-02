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
 * @file cable_id.cpp
 *
 * @brief ipmi-way of discovering cable Ids
 * */

#include "discovery/common.hpp"
#include "discovery/basic/cable_id.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_cable_id.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, enums::Health> discover_cable_id(ipmi::ManagementController& mc, System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetCableId.");

    for (size_t channel = 0; channel < sdv::request::GetCableId::MAX_CHANNEL_NUMBER; channel++) {
        sdv::request::GetCableId request{};
        sdv::response::GetCableId response{};
        request.set_channel(channel);

        try {
            mc.send(request, response);
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetCableId: "
                << e.what()
                << " for System: "
                << system.get_uuid());
            return std::make_tuple(false, enums::Health::Warning);
        }
        const auto cc = response.get_completion_code();

    	if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            log_bad_completion_code(system.get_uuid(), cc, "GetCableId");
            // Unlike other IPMI commands, unsuccessful execution of GetCableId does not always mean
            // errors or problems with hardware. In case there is no cable in the PCIe port, the completion code
            // will be abnormal although an empty port shall be created and added to the manager
            // and the status of the system should not be changed to Warning.
        }
        else {
            system.add_cable_id(response.get_cable_id());
            for (const auto& cable_id : system.get_cable_ids()) {
                log_debug(GET_LOGGER("basic-discovery"), "System: " << system.get_uuid() << ", cable id is " + cable_id);
                log_debug(GET_LOGGER("basic-discovery"), "GetCableId executed successfully.");
            }
        }
    }

    log_info(GET_LOGGER("basic-discovery"), "Cable Ids Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
