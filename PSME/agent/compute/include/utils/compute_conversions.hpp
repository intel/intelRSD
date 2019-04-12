/*!
 * @brief Common compute enum conversions.
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
 * @file compute_conversions.hpp
 */

#pragma once

#include "agent-framework/module/enum/compute.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"

namespace agent {
namespace compute {
namespace utils {

class IscsiMdrConverter {
public:
    /*!
     * @brief Convert model IPAddressType to its iSCSI MDR counterpart.
     *
     * @param at the model IP address type.
     * @return iSCSI MDR IP address type.
     */
    static iscsi::structs::IpAddressType get_ip_type(const agent_framework::model::enums::IPAddressType& at);

    /*!
     * @brief Convert model DHCP status to its iSCSI MDR counterpart.
     *
     * @param enabled the DHCP status.
     * @return iSCSI MDR DHCP status.
     */
    static iscsi::structs::TargetDhcpStatus get_dhcp_enabled(bool enabled);

    /*!
     * @brief Convert model VLAN status to its iSCSI MDR counterpart.
     *
     * @param enabled the VLAN status.
     * @return iSCSI MDR VLAN status.
     */
    static iscsi::structs::VlanStatus get_vlan_enabled(bool enabled);

    /*!
     * @brief Convert model router advertisement status to its iSCSI MDR counterpart.
     *
     * @param enabled the router advertisement status.
     * @return iSCSI MDR router advertisement status.
     */
    static iscsi::structs::RouterAdvertisement get_router_advertisment(bool enabled);

    /*!
     * @brief Convert model authentication method to its iSCSI MDR counterpart.
     *
     * @param am the model authentication method.
     * @return iSCSI MDR authentication method.
     */
    static iscsi::structs::AuthenticationMethod get_authentication_method(const agent_framework::model::enums::FunctionAuthenticationMethod& am);

    /*!
     * @brief Convert iSCSI MDR authentication method to its model counterpart.
     *
     * @param am the iSCSI MDR authentication method.
     * @return model authentication method.
     */
    static agent_framework::model::enums::FunctionAuthenticationMethod get_authentication_method(const iscsi::structs::AuthenticationMethod& am);

    /*!
     * @brief Convert iSCSI MDR authentication method to its model counterpart.
     *
     * @param am the iSCSI MDR authentication method.
     * @return model authentication method.
     */
    static agent_framework::model::enums::IPAddressType get_ip_type(const iscsi::structs::IpAddressType& at);
};

} // namespace utils
} // namespace compute
} // namespace agent
