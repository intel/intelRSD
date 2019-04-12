/*!
 * @brief Declaration of function processing Set Component Attributes command on Network Device Function
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
 * @file set_network_device_function_attributes.hpp
 */

#pragma once

#include "agent-framework/command/compute_commands.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"


namespace agent {
namespace compute {

/*!
 * @brief Implementation of SetComponentAttributes for Network Device Function
 * @param[in] uuid the uuid of the Network Device Function
 * @param[in] attributes the attributes to be set
 * @param response the response to SetComponentAttributes request
 * */
void process_network_device_function(const std::string& uuid,
                                     const agent_framework::model::attribute::Attributes& attributes,
                                     agent_framework::model::responses::SetComponentAttributes& response);


/*!
 * @brief A helper method for setting boot override target to RemoteDrive. It sets iSCSI OOB attributes over IPMI.
 * @param[in] system a reference to the system whose boot options are changed.
 * @param mc the ManagementController with filled connection data used to call IPMI commands
 * */
void set_iscsi_oob_parameters(const agent_framework::model::System& system,
                              ipmi::manager::ipmitool::ManagementController& mc);


/*!
 * @brief A helper method clearing boot override target from RemoteDrive. It clears iSCSI OOB attributes over IPMI.
 * @param[in] system a reference to the system whose boot options are changed.
 * @param mc the ManagementController with filled connection data used to call IPMI commands
 * */
void clear_iscsi_oob_parameters(const agent_framework::model::System& system,
                                ipmi::manager::ipmitool::ManagementController& mc);


/*!
 * @brief Method for checking whether a system's iSCSI OOB attributes are set.
 * @param[in] system a reference to the queried system
 * @return the value of Device Enabled flag on the system's Network Device Function
 * */
bool get_iscsi_enabled(const agent_framework::model::System& system);

}
}
