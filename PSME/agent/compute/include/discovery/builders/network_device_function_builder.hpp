/*!
 * @brief Network device function builder class interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file network_device_function_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/network_device_function.hpp"
#include "iscsi/iscsi_mdr_parser.hpp"


namespace agent {
namespace compute {
namespace discovery {

class NetworkDeviceFunctionBuilder {
public:
    /*!
     * @brief Build default network device function object.
     * @param parent_uuid Parent UUID.
     * @return Default built network device function object.
     */
    static agent_framework::model::NetworkDeviceFunction build_default(const std::string& parent_uuid);

    /*!
     * @brief Update the model with iSCSI MDR data.
     * @param ndf NetworkDeviceFunction object to be filled with discovered data.
     * @param initiator iSCSI data object containing single initiator info.
     * @param target iSCSI data object containing single target info.
     */
    static void update_iscsi_mdr_data(agent_framework::model::NetworkDeviceFunction& ndf,
                                          const mdr::StructEnhanced<iscsi::structs::ISCSI_MDR_INITIATOR>& initiator,
                                          const mdr::StructEnhanced<iscsi::structs::ISCSI_MDR_TARGET>& target);

    /*!
     * @brief Clear the model.
     * @param ndf NetworkDeviceFunction object to be cleared.
     */
    static void clear_iscsi_mdr_data(agent_framework::model::NetworkDeviceFunction& ndf);
};

}
}
}
