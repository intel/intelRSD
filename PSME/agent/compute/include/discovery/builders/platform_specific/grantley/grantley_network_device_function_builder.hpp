/*!
 * @brief Grantley network device function builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file grantley_network_device_function_builder.hpp
 */

#pragma once



#include "discovery/builders/network_device_function_builder.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_control_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_initiator_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_nic_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_target_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/get_oob_heap_boot_options.hpp"
#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_heap_boot_options.hpp"



namespace agent {
namespace compute {
namespace discovery {

class GrantleyNetworkDeviceFunctionBuilder : public NetworkDeviceFunctionBuilder {
public:

    /*!
     * @brief Update network device function object with OOB boot options data.
     * @param network_device_function Network device function object to be filled with discovered data.
     * @param get_oob_control_boot_options_response GetOobControlBootOptions IPMI command response object.
     */
    static void update_oob_control_boot_options(agent_framework::model::NetworkDeviceFunction& network_device_function,
                                                const ipmi::command::sdv::response::GetOobControlBootOptions& get_oob_control_boot_options_response);


    /*!
     * @brief Update network device function object with initiator name data.
     * @param network_device_function Network device function object to be filled with data.
     * @param initiator_name Initiator name.
     */
    static void update_initator_name(agent_framework::model::NetworkDeviceFunction& network_device_function,
                                     const std::string& initiator_name);


    /*!
     * @brief Update network device function object with OOB NIC boot options data.
     * @param network_device_function Network device function object to be filled with discovered data.
     * @param get_oob_nic_boot_options_response GetOobNicBootOptions IPMI command response object.
     */
    static void update_oob_nic_boot_options(agent_framework::model::NetworkDeviceFunction& network_device_function,
                                            const ipmi::command::sdv::response::GetOobNicBootOptions& get_oob_nic_boot_options_response);


    /*!
     * @brief Update Network device function with OOB target boot options.
     * @param network_device_function Network device function object to be filled with discovered data.
     * @param get_oob_target_boot_options GetOobTargetBootOptions IPMI response object.
     * @param primary_target_name Primary target name.
     * @param chap_username CHAP username.
     * @param chap_secret CHAP secret.
     * @param mutual_chap_username Mutual CHAP username.
     * @param mutual_chap_secret Mutual CHAP secret.
     */
    static void update_oob_target_boot_options(agent_framework::model::NetworkDeviceFunction& network_device_function,
                                               const ipmi::command::sdv::response::GetOobTargetBootOptions& get_oob_target_boot_options,
                                               const std::string& primary_target_name,
                                               const std::string& chap_username,
                                               const std::string& chap_secret,
                                               const std::string& mutual_chap_username,
                                               const std::string& mutual_chap_secret);

};

}
}
}
