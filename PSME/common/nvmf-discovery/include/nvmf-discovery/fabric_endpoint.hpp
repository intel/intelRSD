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
 * @file nvmf-discovery/fabric_endpoint.hpp
 */

#pragma once

#include "fabric.hpp"
#include "fabric_exception.hpp"
#include "memory_buffer.hpp"
#include "nvme/definitions.hpp"
#include <atomic>
#include <chrono>
#include <vector>

namespace nvmf_discovery {

/*!
 * Host -> Controller command
 */
union NvmfHost2CtrlMsg {
    nvme::NvmfCapsuleCmd nvmf_cmd;
    nvme::NvmeCmd nvme_cmd;
    nvme::NvmfPropertySetCmd prop_set_cmd;
    nvme::NvmfPropertyGetCmd prop_get_cmd;
    nvme::NvmfConnectCmd connect_cmd;
};

/*!
 * Controller -> Host response
 */
union NvmfCtrl2HostMsg {
    nvme::NvmeCQEntry nvme_cpl;
    nvme::NvmfPropertyGetRsp prop_get_rsp;
    nvme::NvmfConnectRsp connect_rsp;
};

/*!
 * Represents fabric endpoint.
 */
class FabricEndpoint {
public:
    static constexpr int RETRY_COUNT = 100;
    static constexpr TimeoutMs IDLE_TIMEOUT{100};
    static constexpr size_t MAX_QUEUE_DEPTH = 1;

    /*! RDMA connection state */
    enum class RdmaConnectionState {
        DISCONNECTED,
        CONNECTED
    };

    /*!
     * Constructor
     * @param info fi_info structure
     */
    FabricEndpoint(FiInfo info);

    /*! Destructor */
    virtual ~FabricEndpoint();

    FabricEndpoint(const FabricEndpoint& other) = delete;
    FabricEndpoint& operator=(const FabricEndpoint& right) = delete;
    FabricEndpoint(FabricEndpoint&&) = default;
    FabricEndpoint& operator=(FabricEndpoint&&) = default;

    /*!
     * Disconnects from the peer
     */
    void disconnect();

protected:
    FiInfo m_info{};
    FidEq m_eq{};
    FidCq m_rcq{};
    FidCq m_scq{};
    FidEp m_ep{};
    /* buffer to send commands/responses */
    MemoryBuffer m_send{};
    /* posted buffers for incoming data */
    std::vector<MemoryBuffer> m_queue_entries{};
    RdmaConnectionState m_rdma_connection_state{RdmaConnectionState::DISCONNECTED};

    /*!
     * Creates fi_endpoint and queues (receive, completion, event).
     * Allocates MemoryBuffers, associates queues with endpoint and enables endpoint.
     * @param domain Fabric access domain
     * @param fabric Fabric domain
     */
    void init_endpoint(fid_domain& domain, fid_fabric& fabric);

    /*!
     * Remote memory read
     * @param cq completion queue
     * @param buf local data buffer to read into (read target)
     * @param len length of data to read
     * @param desc descriptor associated with local buffer
     * @param addr address of remote memory to access
     * @param key protection key associated with the remote memory
     */
    void rma_read(fid_cq* cq, void* buf, size_t len, void* desc, uint64_t addr, uint64_t key);

    /*!
     * Remote memory write
     * @param cq completion queue
     * @param buf local data buffer to write from (write source)
     * @param len length of data to write
     * @param desc descriptor associated with local buffer
     * @param addr address of remote memory to access
     * @param key protection key associated with the remote memory
     */
    void rma_write(fid_cq* cq, void* buf, size_t len, void* desc, uint64_t addr, uint64_t key);

    /*!
     * Post command
     * @param cmd Command to be send
     * @param bytes size of command
     */
    void post_cmd(void* cmd, size_t bytes);
    void post_set_property(nvme::NvmeCtrlRegisterOffset register_offset, uint64_t val);

    /*!
     * Synchronous read from completion queue
     * @param cq completion queue
     */
    void cq_sread(fid_cq* cq);
};

}
