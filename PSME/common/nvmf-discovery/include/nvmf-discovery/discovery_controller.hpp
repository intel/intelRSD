/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file nvmf-discovery/discovery_controller.hpp
 */

#pragma once

#include "fabric_endpoint.hpp"
#include "discovery_entries_provider.hpp"

namespace nvmf_discovery {

/*!
 * Represents Nvmf request
 */
struct NvmfRequest {
    NvmfRequest(MemoryBuffer* cmd_buf, size_t len, NvmfCtrl2HostMsg* rsp_ptr)
      : cmd_buffer{cmd_buf},
        length{len},
        cmd{reinterpret_cast<NvmfHost2CtrlMsg*>(cmd_buf->buffer.get())},
        rsp{rsp_ptr} {
        *rsp = {};
    }
    MemoryBuffer* cmd_buffer{};
    size_t length{};
    NvmfHost2CtrlMsg* cmd{};
    NvmfCtrl2HostMsg* rsp{};
};

/*!
 * Represents Discovery Controller handling discovery requests
 */
class DiscoveryController : public FabricEndpoint {
public:

    /*!
     * Constructor
     * @param info fi_info structure pointer
     * @param domain fabric access domain
     * @param fabric fabric domain
     * @param discovery_entries_provider Discovery entries provider
     */
    DiscoveryController(FiInfo info, fid_domain& domain, fid_fabric& fabric,
                        DiscoveryEntriesProvider::Ptr discovery_entries_provider);

    /*! Destructor */
    ~DiscoveryController() override;

    /*!
     * Nvmf requests processor
     */
    void handle_requests();

private:
    void accept_connection();
    void handle_request(NvmfRequest& request);
    void handle_connect(NvmfRequest& request);
    void handle_property_get(NvmfRequest& request);
    void handle_property_set(NvmfRequest& request);
    void handle_identify(NvmfRequest& request);
    void handle_get_log_pages(NvmfRequest& request);
    void send_response_and_repost(NvmfRequest& request);

    MemoryBuffer m_data{};
    fid_domain& m_domain;
    // contains hostnqn to which we are connected
    std::string m_host_nqn{};
    uint32_t m_controller_status{};
    DiscoveryEntriesProvider::Ptr m_discovery_entries_provider;
};

}
