/*!
 * @brief OPAE proxy host API declaration
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file opae_proxy_host_api.hpp
 */

#pragma once



#include "agent-framework/module/utils/utils.hpp"
#include "opae_proxy_context.hpp"
#include "opae-proxy/acl.h"
#include "opae-proxy/manage.h"
#include "opae-proxy/access.h"
#include <vector>



namespace opaepp {

class OpaeProxyHostApi {
public:
    /*!
     * @brief Adds initiator host to opae proxy using specific opae proxy context
     * @param opae_proxy_context opae proxy context wrapped
     * @param host_uuid uuid of the host to add to opae proxy
     */
    static void add_initiator_host(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid);


    /*!
     * @brief Removes initiator host from opae proxy using specific opae proxy context
     * @param opae_proxy_context opae proxy context wrapped
     * @param host_uuid uuid of the host to remove from opae proxy
     */
    static void remove_initiator_host(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid);


    /*!
     * @brief Gets all initiator host from the opae proxy
     * @param opae_proxy_context opae proxy context wrapped
     * @param max_hosts maximum number of host to return
     * @return vector of initiator hosts uuids
     */
    static std::vector<Uuid> get_initiator_hosts(OpaeProxyContext& opae_proxy_context, const uint32_t max_hosts);


    /*!
     * @brief Sets ownership of the device for the opae proxy in given context
     * @param opae_proxy_context opae proxy context wrapped
     * @param host_uuid uuid of new owner host of the device
     * @param processor_uuid uuid of the device to be owned by given host
     */
    static void
    set_device_ownership(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid, const Uuid& processor_uuid);


    /*!
     * @brief Removes ownership of the device for the opae proxy in given context
     * @param opae_proxy_context opae proxy context wrapped
     * @param processor_uuid uuid of the device to be unowned
     */
    static void remove_device_ownership(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid);


    /*!
     * @brief Gets uuid of the host owner of the specific device
     * @param opae_proxy_context opae proxy context wrapped
     * @param processor_uuid uuid of the device which owner device is to be found
     * @return Uuid of the device owner host
     */
    static OptionalField<Uuid>
    get_device_owner_host_id(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid);


    /*!
     * @brief Reconfigures FPGA AFU by deploying new green bit stream
     * @param opae_proxy_context opae proxy context wrapped
     * @param processor_uuid uuid of the device which AFU has to be reconfigured
     * @param bitstream_bytes vector of bytes with new green bit stream
     */
    static void reconfigure_slot(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid,
                                 const std::vector<uint8_t>& bitstream_bytes);


    /*!
     * @brief Returns opae fpga token for given processor uuid from model
     * @param opae_proxy_context opae proxy context wrapped
     * @param processor_uuid processor's uuid from the model
     * @param command_name name of the opae-proxy host API command in which this method is called
     * @return fpga token for given device uuid
     */
    static fpga_token& get_token_from_uuid(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid,
                                           const std::string& command_name);
};

}
