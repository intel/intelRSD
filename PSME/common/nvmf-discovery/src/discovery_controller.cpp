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
 * @file nvmf-discovery/discovery_controller.cpp
 */

#include "nvmf-discovery/discovery_controller.hpp"
#include "logger/logger_factory.hpp"
#include "utils/hex_dump.hpp"

#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace nvme;

namespace {

/*! @return Size of array */
template<typename T, size_t N>
constexpr size_t array_size(T(&)[N]) { return N; }

constexpr int BYTES_IN_DWORD = 4;

constexpr uint64_t CONTROLLER_CAPABILITIES = 0x200f0003ffL;
// sgls supported, keyed sgl, sgl offset
constexpr uint32_t SGL_SUPPORT = 0x100005;
// extended data for get log page
constexpr uint8_t LOG_PAGE_ATTRIBUTES = 0x04;

}

namespace nvmf_discovery {

DiscoveryController::DiscoveryController(FiInfo finfo, fid_domain& domain, fid_fabric& fabric,
        DiscoveryEntriesProvider::Ptr discovery_entries_getter)
    : FabricEndpoint(std::move(finfo)),
      m_domain(domain),
      m_discovery_entries_provider(discovery_entries_getter) {
    init_endpoint(m_domain, fabric);
    accept_connection();
    m_send.allocate_and_register(m_domain, MemoryBuffer::DEFAULT_SIZE);
    m_data.allocate_and_register(m_domain, MemoryBuffer::DEFAULT_SIZE);
}


DiscoveryController::~DiscoveryController() {

}

void DiscoveryController::accept_connection() {
    fi_eq_cm_entry eq_cm_entry{};
    uint32_t event{};
    ssize_t ret{};

    for (auto& elem: m_queue_entries) {
        ret = fi_recv(m_ep.get(), elem.buffer.get(), elem.length, fi_mr_desc(elem.mr.get()), 0, &elem);
        if (ret) {
            throw FabricException("fi_recv error: " + std::string(fi_strerror(int(ret))), ret);
        }
    }

    ret = fi_accept(m_ep.get(), nullptr, 0);
    if (ret) {
        throw FabricException("fi_accept error: " + std::string(fi_strerror(int(ret))), ret);
    }

    ret = fi_eq_sread(m_eq.get(), &event, &eq_cm_entry, sizeof(eq_cm_entry), IDLE_TIMEOUT.count(), 0);
    if (ret != sizeof(eq_cm_entry)) {
        throw FabricException("fi_eq_sread error: " + get_eq_error_string(m_eq.get(), ret), ret);
    }

    if (event != FI_CONNECTED) {
        throw FabricException("unexpected event " + std::to_string(event), 0);
    }

    m_rdma_connection_state = FabricEndpoint::RdmaConnectionState::CONNECTED;
}

void DiscoveryController::handle_connect(NvmfRequest& request) {
    log_debug("nvmf-discovery", "handle_connect");

    uint64_t addr = request.cmd->nvme_cmd.data_pointer.sgl1.address;
    uint64_t key = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.key;
    uint64_t len = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.length;

    NvmfConnectData* connect_data = reinterpret_cast<NvmfConnectData*>(m_data.buffer.get());
    void* desc = fi_mr_desc(m_data.mr.get());

    // read connect command data
    rma_read(m_scq.get(), connect_data, len, desc, addr, key);

    m_host_nqn.reserve(SUBSYSTEM_NQN_SIZE);
    for (int i = 0; connect_data->hostnqn[i] && i < SUBSYSTEM_NQN_SIZE; ++i) {
        m_host_nqn.push_back(std::string::value_type(connect_data->hostnqn[i]));
    }
    log_debug("nvmf-discovery", "host " << m_host_nqn << " connected");

    if (strcmp(DiscoverySubsystemNqn, reinterpret_cast<char*>(connect_data->subnqn))) {
        nvme::NvmeStatus status{};
        status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
        status.status_code = static_cast<uint8_t>(NvmeGenericCommandStatusCode::InvalidField);
        status.do_not_retry = 1;
        throw NvmfException("Invalid subsystem nqn", std::move(status));
    }
    if (connect_data->cntlid != CONTROLLER_ID_DYNAMIC) {
        nvme::NvmeStatus status{};
        status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
        status.status_code = static_cast<uint8_t>(NvmeGenericCommandStatusCode::InvalidField);
        status.do_not_retry = 1;
        throw NvmfException("Invalid controller start_idx", std::move(status));
    }
}

void DiscoveryController::handle_property_get(NvmfRequest& request) {
    log_debug("nvmf-discovery", "handle_property_get");
    uint64_t value;

    auto offset = request.cmd->prop_get_cmd.ofst;
    if (offset == NvmeCtrlRegisterOffset::ControllerStatus) {
        value = m_controller_status;
    }
    else if (offset == NvmeCtrlRegisterOffset::Capabilities) {
        value = CONTROLLER_CAPABILITIES;
    }
    else if (offset == NvmeCtrlRegisterOffset::Version) {
        value = static_cast<uint32_t>(NvmeVersion::NVME_1_2_1);
    }
    else {
        nvme::NvmeStatus status{};
        status.do_not_retry = 1;
        throw NvmfException("Invalid property requested", std::move(status));
    }

    request.rsp->prop_get_rsp.value.u64 = value;
}

void DiscoveryController::handle_property_set(NvmfRequest& request) {
    log_debug("nvmf-discovery", "handle_property_set");
    static constexpr uint64_t NVME_CTRL_ENABLE = 0x460001;
    static constexpr uint32_t SHUTDOWN_COMPLETE = 0x08;
    static constexpr uint32_t READY = 0x01;

    if (request.cmd->prop_set_cmd.ofst == NvmeCtrlRegisterOffset::ControllerConfiguration) {
        m_controller_status = (request.cmd->prop_set_cmd.value.u64 == NVME_CTRL_ENABLE)
                            ? READY : SHUTDOWN_COMPLETE;
    }
    else {
        nvme::NvmeStatus status{};
        status.do_not_retry = 1;
        throw NvmfException("Invalid property requested", std::move(status));
    }
}

void DiscoveryController::handle_identify(NvmfRequest& request) {
    log_debug("nvmf-discovery", "handle_identify");
    ControllerData *start_idx = reinterpret_cast<ControllerData*>(m_data.buffer.get());
    *start_idx = {};
    void* desc = fi_mr_desc(m_data.mr.get());

    uint64_t addr = request.cmd->nvme_cmd.data_pointer.sgl1.address;
    uint64_t key = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.key;
    uint64_t length = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.length;

    if (request.cmd->nvme_cmd.identify.cns != uint8_t(IdentifyCns::Controller)) {
        nvme::NvmeStatus status{};
        status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
        status.status_code = static_cast<uint8_t>(NvmeGenericCommandStatusCode::InvalidField);
        status.do_not_retry = 1;
        throw NvmfException("Invalid field requested", std::move(status));
    }

    start_idx->mdts = 0;
    start_idx->controller_id = 0;
    start_idx->version = static_cast<uint32_t>(NvmeVersion::NVME_1_2_1);
    start_idx->lpa = LOG_PAGE_ATTRIBUTES;
    start_idx->maxcmd = FabricEndpoint::MAX_QUEUE_DEPTH;
    start_idx->sgls = SGL_SUPPORT;
    std::copy_n(DiscoverySubsystemNqn, array_size(DiscoverySubsystemNqn), start_idx->subsystem_nqn);

    if (length > sizeof(*start_idx)) {
        length = sizeof(*start_idx);
    }

    rma_write(m_scq.get(), start_idx, length, desc, addr, key);
}

void DiscoveryController::handle_get_log_pages(NvmfRequest& request) {
    uint64_t addr = request.cmd->nvme_cmd.data_pointer.sgl1.address;
    uint64_t key = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.key;
    uint64_t keyed_length = request.cmd->nvme_cmd.data_pointer.sgl1.keyed.length;
    uint64_t response_length = (request.cmd->nvme_cmd.get_log_page.num_of_dwords + 1) * BYTES_IN_DWORD;

    // Validating GetLogPage request.
    if (response_length > keyed_length) {
        nvme::NvmeStatus status{};
        status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
        status.status_code = static_cast<uint8_t>(NvmeGenericCommandStatusCode::InvalidField);
        status.do_not_retry = 1;
        throw NvmfException("Requested length " + std::to_string(response_length)
                             + " greater than buffer size " + std::to_string(keyed_length), std::move(status));
    }

    auto buffer_size = m_data.length;

    MemoryBuffer* buffer = &m_data;
    MemoryBuffer buf{};
    if (response_length  > buffer_size) {
        // MemoryBuffer uses posix_memalign allocation which requires memory to be aligned to 2^n address.
        auto size = pow(2, ceil(log(double(keyed_length))/log(2)));
        buf.allocate_and_register(m_domain, size_t(size));
        buffer = &buf;
        log_debug("nvmf-discovery", "Reallocate to size: " << size_t(size));
    }

    auto* log = reinterpret_cast<LogPageDiscoveryHeader*>(buffer->buffer.get());
    log->number_of_records = m_discovery_entries_provider->get_discovery_entries_count(m_host_nqn);
    log->generation_counter = m_discovery_entries_provider->get_generation_counter(m_host_nqn);

    if (keyed_length > sizeof(LogPageDiscoveryHeader)) {
        const auto discovery_entries = m_discovery_entries_provider->get_discovery_entries(m_host_nqn);

        // According to NVMe over Fabrics 1.0a Discovery Service must return RestartDiscovery status code
        // if number of entries change during 
        if (discovery_entries.size() != log->number_of_records ) {
            log_info("nvmf-discovery", " number of records changed, restart discovery");
            request.rsp->nvme_cpl.status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::CommandSpecific);
            request.rsp->nvme_cpl.status.status_code = static_cast<uint8_t>(NvmfCommandStatusCode::RestartDiscovery);
            return;
        }

        auto start_idx = uint64_t(request.cmd->nvme_cmd.get_log_page.log_page_offset / sizeof(LogPageDiscoveryHeader));
        auto stop_idx = start_idx + uint64_t((response_length - 1) / sizeof(LogPageDiscoveryHeader));

        auto* entries_buffer = reinterpret_cast<LogPageDiscoveryEntry*>(&log[1]);
        for (uint64_t j = start_idx; j < stop_idx; j++) {
            const auto& entry = discovery_entries[j];

            entries_buffer->transport_type = entry.transport_type;
            entries_buffer->treq_secure_channel = entry.treq_secure_channel;
            entries_buffer->address_family = entry.address_family;
            entries_buffer->controller_id = entry.controller_id;
            entries_buffer->tsas.rdma.qptype = RdmaQPType::ReliableConnected;
            entries_buffer->tsas.rdma.prtype = RdmaProviderType::None;
            entries_buffer->tsas.rdma.cms = RdmaCMS::RdmaCm;
            entries_buffer->subsystem_type = entry.subsystem_type;
            entries_buffer->admin_max_sq_size = entry.admin_max_sq_size;

            std::copy(std::begin(entry.subsystem_nqn), std::end(entry.subsystem_nqn), entries_buffer->subsystem_nqn);
            std::copy(std::begin(entry.transport_address), std::end(entry.transport_address), entries_buffer->transport_address);
            std::copy(std::begin(entry.transport_service_id), std::end(entry.transport_service_id), entries_buffer->transport_service_id);

            entries_buffer++;

            log_debug("nvmf-discovery", " subnqn: " << entry.subsystem_nqn << " at " << entry.transport_address << ":"
                                                    << entry.transport_service_id);
        }
    }

    rma_write(m_scq.get(), log, keyed_length, fi_mr_desc(buffer->mr.get()), addr, key);
    log_debug("nvmf-discovery", "Sent GetLogPage response: " << m_data.length << " bytes.");
}

void DiscoveryController::handle_request(NvmfRequest& request) {
    auto& nvme_cmd = request.cmd->nvme_cmd;
    auto& nvme_cpl = request.rsp->nvme_cpl;

    nvme_cpl.command_id = nvme_cmd.command_id;

    auto opcode = nvme_cmd.opcode;
    try {
        if (opcode == NvmeFabricsCommandOpcode) {
            const auto fctype = request.cmd->nvmf_cmd.fctype;
            switch (fctype) {
            case NvmfCommandType::PropertySet:
                handle_property_set(request);
                break;
            case NvmfCommandType::PropertyGet:
                handle_property_get(request);
                break;
            case NvmfCommandType::Connect:
                handle_connect(request);
                break;
            case NvmfCommandType::AuthenticationSend:
            case NvmfCommandType::AuthenticationRecv:
            case NvmfCommandType::StartVendorSpecific:
            default:
                log_error("nvmf-discovery", "Unsupported fabric command type: " << uint(fctype));
                nvme_cpl.status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
                nvme_cpl.status.status_code = static_cast<uint8_t>(NvmfCommandStatusCode::InvalidParam);
                nvme_cpl.status.do_not_retry = 1;
            }
        }
        else if (opcode == uint8_t(AdminCommandOpcode::Identify)) {
            handle_identify(request);
        }
        else if (opcode == uint8_t(AdminCommandOpcode::KeepAlive)) {
            log_info("nvmf-discovery", "KeepAlive");
        }
        else if (opcode == uint8_t(AdminCommandOpcode::GetLogPage)) {
            handle_get_log_pages(request);
        }
        else {
            log_error("nvmf-discovery", "Unsupported nvme opcode " << opcode);
            nvme_cpl.status.status_code_type = static_cast<uint8_t>(NvmeStatusCodeType::Generic);
            nvme_cpl.status.status_code = static_cast<uint8_t>(NvmeGenericCommandStatusCode::InvalidOpcode);
            nvme_cpl.status.do_not_retry = 1;
        }
    }
    catch (const NvmfException& e) {
        log_error("nvmf-discovery", e.what());
        nvme_cpl.status = e.get_status();
    }
    catch (const std::exception& e) {
        log_error("nvmf-discovery", e.what());
        nvme_cpl.status.do_not_retry = 1;
    }

    send_response_and_repost(request);
}

void DiscoveryController::handle_requests() {
    log_debug("nvmf-discovery", "started processing requests");
    fi_cq_err_entry cq_entry{};
    fi_eq_cm_entry eq_entry{};
    uint32_t event{};
    int retry_count = RETRY_COUNT;

    while (retry_count) {
        auto ret = fi_eq_read(m_eq.get(), &event, &eq_entry, sizeof(eq_entry), 0);
        if (ret == sizeof(eq_entry) && event == FI_SHUTDOWN) {
            log_debug("nvmf-discovery", "received shutdown event ");
            break;
        }
        ret = fi_cq_sread(m_rcq.get(), &cq_entry, 1, nullptr, IDLE_TIMEOUT.count());
        if (ret > 0) {
            NvmfRequest request(reinterpret_cast<MemoryBuffer*>(cq_entry.op_context), cq_entry.len,
                                reinterpret_cast<NvmfCtrl2HostMsg*>(m_send.buffer.get()));
            log_debug("nvmf-discovery", utils::hex_dump(reinterpret_cast<unsigned char*>(request.cmd_buffer->buffer.get()),
                                                 0, request.length));
            try {
                handle_request(request);
            }
            catch (const std::exception& e) {
                log_error("nvmf-discovery", e.what());
            }
        }
        else if (ret == -EAGAIN) {
            log_debug("nvmf-discovery", "waiting on nvmf command " << retry_count);
            retry_count--;
        }
        else if (ret != -EINTR) {
            log_error("nvmf-discovery", get_cq_error_string(m_rcq.get(), ret));
            break;
        }
    }
    if (event != FI_SHUTDOWN) {
        disconnect();
    }

    log_debug("nvmf-discovery", "stopped processing requests");
}

void DiscoveryController::send_response_and_repost(NvmfRequest& request) {

    auto ret = fi_recv(m_ep.get(), request.cmd_buffer->buffer.get(), request.cmd_buffer->length,
                       fi_mr_desc(request.cmd_buffer->mr.get()), 0, request.cmd_buffer);
    if (ret) {
        throw FabricException("fi_recv error: " + fi_error_to_string(int(ret)), ret);
    }
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
    ret = fi_send(m_ep.get(), request.rsp, sizeof(*request.rsp), fi_mr_desc(m_send.mr.get()), FI_ADDR_UNSPEC, nullptr);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    if (ret) {
        throw FabricException("fi_send error: " + fi_error_to_string(int(ret)), ret);
    }

    cq_sread(m_scq.get());
}

}
