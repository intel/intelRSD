/*!
 * @brief OPAE proxy wrapper context class declaration
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
 * @file opae_proxy_context.hpp
 */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "opae-proxy/backend.h"
#include "opae-proxy/types.h"
#include <map>
#include "opaepp/opae-proxy/opae_proxy_transport_detail.hpp"



namespace opaepp {

using ModelUuidToTokenMap = std::map<Uuid, fpga_token>;
using ModelUuidToFpgaUniqueKeyMap = std::map<Uuid, std::string>;
using FpgaUniqueKeyToTokenPair = std::pair<std::string, fpga_token>;
using FpgaUniqueKeyToTokenMap = std::map<std::string, fpga_token>;

/*!
 * OpaeProxyContext declaration
 */
class OpaeProxyContext {
public:
    /*!
     * @brief Default constructor
     * @param ip_address IP address for opae-proxy to use
     * @param port port for opae-proxy to use
     * @param transports List of transports for opae-proxy to use
     */
    OpaeProxyContext(const std::vector<opaepp::TransportDetails>& transports);


    /*!
     * @brief Default destructor
     */
    ~OpaeProxyContext();


    /*!
     * @brief Get underlying fpgaof structure
     * @return pointer to fpgaof structure in this context
     */
    struct fpgaof* get_backend();


    /*!
     * @brief Get map of <processor uuid, fpga token> pairs which is mapping of model devices into opae devices
     * @return map of <processor uuid, fpga token> pairs for opae proxy wrapper context
     */
    ModelUuidToTokenMap& get_proxy_device_ids();


    /*!
     * @brief Sets mapping of FPGA devices from model uuid to opae fpga_token
     * @param model_uuid_to_opae_uuid_map container which maps FPGAs model uuid to unique uuid from device
     */
    void set_model_to_token_mapping(ModelUuidToFpgaUniqueKeyMap& model_uuid_to_opae_uuid_map);


    /*!
     * @brief Sets mapping of unique uuid from FPGA to FPGA token from opae
     * @param fpga_unique_key_to_token_map container which maps unique uuid from FPGA to opae fpga token
     */
    void set_enumerated_devices_mapping(const FpgaUniqueKeyToTokenMap& fpga_unique_key_to_token_map);


private:
    static constexpr const uint32_t m_fpgaof_backend_version = 1;

    struct fpgaof m_backend{m_fpgaof_backend_version, nullptr};
    ModelUuidToTokenMap m_proxy_device_id_map{};
    FpgaUniqueKeyToTokenMap m_fpga_unique_key_to_token_map{};
};

using OpaeProxyContextPtr = std::shared_ptr<OpaeProxyContext>;

}