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
 * @file nvmf-discovery/fabric_endpoint.cpp
 */

#include "nvmf-discovery/fabric_endpoint.hpp"
#include "logger/logger_factory.hpp"

#include <stdexcept>
#include <cstring>

namespace nvmf_discovery {

constexpr TimeoutMs FabricEndpoint::IDLE_TIMEOUT;

FabricEndpoint::FabricEndpoint(FiInfo provider)
    : m_info{std::move(provider)} {
}

FabricEndpoint::~FabricEndpoint() {}


void FabricEndpoint::init_endpoint(fid_domain& domain, fid_fabric& fabric) {

    for (auto i = 0u; i < MAX_QUEUE_DEPTH; i++) {
        MemoryBuffer buffer{};
        buffer.allocate_and_register(domain, MemoryBuffer::DEFAULT_SIZE);
        m_queue_entries.push_back(std::move(buffer));
    }

    fi_eq_attr eq_attr = {};
    fi_cq_attr cq_attr = {};

    m_info->ep_attr->tx_ctx_cnt = 1;
    m_info->ep_attr->rx_ctx_cnt = 2;
    m_info->tx_attr->iov_limit = 1;
    m_info->rx_attr->iov_limit = 1;
    m_info->tx_attr->inject_size = 0;

    cq_attr.size = MAX_QUEUE_DEPTH;
    cq_attr.format = FI_CQ_FORMAT_MSG;
    cq_attr.wait_obj = FI_WAIT_UNSPEC;
    cq_attr.wait_cond = FI_CQ_COND_NONE;

    fid_cq* rc_q{};
    auto ret = fi_cq_open(&domain, &cq_attr, &rc_q, nullptr);
    m_rcq.reset(rc_q);
    if (ret) {
        throw FabricException("fi_cq_open for rcq error: " + fi_error_to_string(int(ret)), ret);
    }

    fid_cq* sc_q{};
    ret = fi_cq_open(&domain, &cq_attr, &sc_q, nullptr);
    m_scq.reset(sc_q);
    if (ret) {
        throw FabricException("fi_cq_open for scq error: " + fi_error_to_string(int(ret)), ret);
    }

    fid_eq* e_q{};
    ret = fi_eq_open(&fabric, &eq_attr, &e_q, nullptr);
    m_eq.reset(e_q);
    if (ret) {
        throw FabricException("fi_eq_open error: " + fi_error_to_string(int(ret)), ret);
    }

    fid_ep* endpoint{};
    ret = fi_endpoint(&domain, m_info.get(), &endpoint, this);
    m_ep.reset(endpoint);
    if (ret) {
        throw FabricException("fi_endpoint error: " + fi_error_to_string(int(ret)), ret);
    }

    ret = fi_ep_bind(m_ep.get(), &m_rcq->fid, FI_RECV);
    if (ret) {
        throw FabricException("fi_send for rcq error: " + fi_error_to_string(int(ret)), ret);
    }

    ret = fi_ep_bind(m_ep.get(), &m_scq->fid, FI_SEND);
    if (ret) {
        throw FabricException("fi_ep_bind for scq error: " + fi_error_to_string(int(ret)), ret);
    }

    ret = fi_ep_bind(m_ep.get(), &m_eq->fid, 0);
    if (ret) {
        throw FabricException("fi_ep_bind for eq error: " + fi_error_to_string(int(ret)), ret);
    }

    ret = fi_enable(m_ep.get());
    if (ret) {
        throw FabricException("fi_enable error: " + fi_error_to_string(int(ret)), ret);
    }
}

void FabricEndpoint::cq_sread(fid_cq* cq) {
    fi_cq_err_entry cq_entry{};
    auto retry_count = FabricEndpoint::RETRY_COUNT;
    while(retry_count) {
        auto ret = fi_cq_sread(cq, &cq_entry, 1, nullptr, FabricEndpoint::IDLE_TIMEOUT.count());
        if (ret > 0) {
            return;
        }
        if (ret == -EAGAIN || ret == -EINTR) {
            log_debug("nvmf", "cq_read retry" << int(ret));
            retry_count--;
            if (!retry_count) {
                throw FabricException("cq_read timeout", int(ret));
            }
        }
        throw FabricException(get_cq_error_string(cq, ret), ret);
    };
}

void FabricEndpoint::rma_read(fid_cq* cq, void* buf, size_t len, void* desc, uint64_t addr, uint64_t key) {
    auto ret = fi_read(m_ep.get(), buf, len, desc, 0, addr, key, nullptr);
    if (ret) {
        throw FabricException("rma_read error: " + fi_error_to_string(int(ret)), ret);
    }
    cq_sread(cq);
}

void FabricEndpoint::rma_write(fid_cq* cq, void* buf, size_t len, void* desc, uint64_t addr, uint64_t key) {
    auto ret = fi_write(m_ep.get(), buf, len, desc, 0, addr, key, nullptr);
    if (ret) {
        throw FabricException("rma_write error: " + fi_error_to_string(int(ret)), ret);
    }
    cq_sread(cq);
}


void FabricEndpoint::post_cmd(void* command, size_t bytes) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
    auto ret = fi_send(m_ep.get(), command, bytes, fi_mr_desc(m_send.mr.get()), FI_ADDR_UNSPEC, nullptr);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    if (ret) {
        throw FabricException("fi_send error: " + fi_error_to_string(int(ret)), ret);
    }
}

void FabricEndpoint::post_set_property(nvme::NvmeCtrlRegisterOffset register_offset, uint64_t val) {
    static constexpr uint8_t NVME_CMD_SGL_METABUF = (1 << 6);
    NvmfHost2CtrlMsg* cmd = reinterpret_cast<NvmfHost2CtrlMsg*>(m_send.buffer.get());
    auto data_ptr = reinterpret_cast<uintptr_t>(&cmd[1]);
    memset(cmd, 0, m_send.length);
    cmd->prop_set_cmd.opcode = nvme::NvmeFabricsCommandOpcode;
    cmd->prop_set_cmd.reserved0 = NVME_CMD_SGL_METABUF;
    cmd->prop_set_cmd.fctype = nvme::NvmfCommandType::PropertySet;
    cmd->prop_set_cmd.ofst = register_offset;
    cmd->prop_set_cmd.value.u64 = htole64(val);
    cmd->nvme_cmd.data_pointer.sgl1.address = data_ptr;
    cmd->nvme_cmd.data_pointer.sgl1.keyed.length = (m_send.length & 0xFFFFFF);
    cmd->nvme_cmd.data_pointer.sgl1.keyed.key = uint32_t(fi_mr_key(m_send.mr.get()));
    cmd->nvme_cmd.data_pointer.sgl1.keyed.type = (uint8_t(nvme::SglDescriptorType::KeyedDataBlock) & 0xF);

    post_cmd(cmd, sizeof(*cmd));
}

void FabricEndpoint::disconnect() {
    static constexpr uint64_t NVME_CTRL_DISABLE = 0x464001;
    if (m_rdma_connection_state == FabricEndpoint::RdmaConnectionState::CONNECTED) {
        post_set_property(nvme::NvmeCtrlRegisterOffset::Capabilities, NVME_CTRL_DISABLE);
        fi_shutdown(m_ep.get(), 0);
        m_rdma_connection_state = FabricEndpoint::RdmaConnectionState::DISCONNECTED;
    }
}

}
