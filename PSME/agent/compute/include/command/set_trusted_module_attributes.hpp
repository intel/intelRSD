/*!
 * @brief Declaration of function processing Set Component Attributes command on Trusted Module
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
 * @file set_trusted_module_attributes.hpp
 */

#pragma once

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"


namespace agent {
namespace compute {

/*!
 * @brief Implementation of SetComponentAttributes for Trusted Module
 * @param[in] uuid the uuid of the Trusted Module
 * @param[in] attributes the attributes to be set
 * @param response the response to SetComponentAttributes request
 * */
void process_trusted_module(const std::string& uuid,
                            const agent_framework::model::attribute::Attributes& attributes,
                            agent_framework::model::responses::SetComponentAttributes& response);
}
}
