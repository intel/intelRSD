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
 * @file common.hpp
 *
 * @brief common helpers used during discovery
 * */

#pragma once

#include "ipmi/response.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/chassis.hpp"
#include "agent-framework/generic/obj_reference.hpp"
#include <string>

namespace agent {
namespace compute {
namespace discovery {

/*!
 * @brief Wrapper for logging and runtime_error exception
 * */
void log_bad_completion_code(const std::string& uuid, ipmi::Response::COMPLETION_CODE cc, const std::string& command);

/*!
 * @brief Wrapper for logging and runtime_error exception
 * */
void log_bad_completion_code(ipmi::Response::COMPLETION_CODE cc, const std::string& command);

template<typename T>
void update_health(T& resource, agent_framework::model::enums::Health health) {
    if (health != agent_framework::model::enums::Health::OK) {
        auto status = resource.get_status();
        status.set_health(health);
        resource.set_status(status);
    }
}

std::vector<agent_framework::generic::ObjReference<agent_framework::model::Chassis, std::recursive_mutex>>
get_chassis_refs(const std::string& parent_manager_uuid);

bool discover_rrc_network_interface(agent_framework::model::System& system);

}
}
}
