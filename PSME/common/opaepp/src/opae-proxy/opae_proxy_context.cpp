/*!
 * @brief OPAE proxy wrapper context class implementation
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
 * @file opae_proxy_context.cpp
 */


#include "opaepp_error.hpp"
#include "opaepp/opae-proxy/opae_proxy_context.hpp"



using namespace opaepp;

namespace {
void configure_transports(fpgaof& backend, const std::vector<TransportDetails>& transports) {

    for (const auto& transport: transports) {
        auto result = fpgaofAddTransport(&backend,
                                         transport.get_protocol_name().c_str(),
                                         transport.get_ipv4_address().c_str(),
                                         transport.get_port());
        OpaeApiError::throw_if_unexpected_result(result, "fpgaofAddTransport",
                                                 "Could not add transport in OpaeProxyContext.");

        if (result == FPGA_OK) {
            log_info("fpgaof-agent", "Registered FPGA-oF transport:"
                << "\n\tProtocol: " << transport.get_protocol_name()
                << "\n\tIP:       " << transport.get_ipv4_address()
                << "\n\tPort:     " << transport.get_port()
            );
        }
    }
}
}


OpaeProxyContext::OpaeProxyContext(const std::vector<opaepp::TransportDetails>& transports) {
    auto result = fpgaofInitBackend(&m_backend);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofInitBackend",
                                             "Could not initialize fpgaof backend structure in OpaeProxyContext.");

    configure_transports(m_backend, transports);

    result = fpgaofServerStart(&m_backend);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofServerStart",
                                             "Could not start fpgaof server in OpaeProxyContext.");
    if (result == FPGA_OK) {
        log_info("fpgaof-agent", "Started FPGA-oF target server.");
    }
}


OpaeProxyContext::~OpaeProxyContext() {
    auto result = fpgaofServerStop(&m_backend);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofServerStop",
                                             "Could not stop fpgaof server in OpaeProxyContext.");

    result = fpgaofCloseBackend(&m_backend);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofCloseBackend",
                                             "Could not close backend fpgoaf structure in OpaeProxyContext.");
}


struct fpgaof* OpaeProxyContext::get_backend() {
    return &m_backend;
}


ModelUuidToTokenMap& OpaeProxyContext::get_proxy_device_ids() {
    return m_proxy_device_id_map;
}


void OpaeProxyContext::set_model_to_token_mapping(ModelUuidToFpgaUniqueKeyMap& model_uuid_to_fpga_unique_key_map) {
    m_proxy_device_id_map.clear();
    for (const auto& mapping_pair : model_uuid_to_fpga_unique_key_map) {
        auto fpga_unique_key = mapping_pair.second;
        auto it = m_fpga_unique_key_to_token_map.find(fpga_unique_key);
        if (it != std::end(m_fpga_unique_key_to_token_map)) {
            m_proxy_device_id_map[mapping_pair.first] = it->second;
        }
        else {
            throw OpaeppError(
                "Opaepp error: opae proxy context cannot find FPGA token for FPGA device " + mapping_pair.first);
        }
    }
}


void OpaeProxyContext::set_enumerated_devices_mapping(const FpgaUniqueKeyToTokenMap& fpga_unique_key_to_token_map) {
    m_fpga_unique_key_to_token_map = fpga_unique_key_to_token_map;
}
