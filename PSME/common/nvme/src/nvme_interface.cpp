/*!
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
 * @file nvme/nvme_interface.cpp
 */

#include "nvme/nvme_interface.hpp"
#include "nvme/commands/controller_reset.hpp"
#include "nvme/commands/flush.hpp"
#include "nvme/commands/format_nvm.hpp"
#include "nvme/commands/get_log_page.hpp"
#include "nvme/commands/identify.hpp"
#include "nvme/commands/namespace_attachment.hpp"
#include "nvme/commands/namespace_management.hpp"
#include "nvme/commands/get_features.hpp"
#include "nvme/commands/set_features.hpp"


using namespace nvme;
using namespace nvme::commands;


NvmeInterface::NvmeInterface(std::shared_ptr<AbstractNvmeInvoker> invoker)
    : AbstractNvmeInterface(invoker) {}


NvmeInterface::~NvmeInterface() {}


void NvmeInterface::reset(const std::string& target) const {
    ControllerReset cmd{};
    m_invoker->execute(target, cmd);
}


void NvmeInterface::flush(const std::string& target, uint32_t namespace_id) const {
    Flush cmd{namespace_id};
    m_invoker->execute(target, cmd);
}


void NvmeInterface::format(const std::string& target, uint32_t namespace_id, FormatNvmSes format_type) const {
    FormatNvm cmd{namespace_id, format_type};
    m_invoker->execute(target, cmd);
}


LogPageSmart NvmeInterface::get_smart_log(const std::string& target, uint32_t namespace_id) const {
    GetLogPage cmd{namespace_id, LogPageId::Smart};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_smart_log());
}


LogPageFirmware NvmeInterface::get_firmware_log(const std::string& target, uint32_t namespace_id) const {
    GetLogPage cmd{namespace_id, LogPageId::Firmware};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_firmware_log());
}

LogPageIOQueues NvmeInterface::get_ioq_log(const std::string& target, std::uint32_t namespace_id) const {
    GetLogPage cmd{namespace_id, LogPageId::IntelVendorIOQsMetrics};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_io_queues_log());
}

LogPageLatencyStats NvmeInterface::get_write_latency_histogram(const std::string& target,
                                                               std::uint32_t namespace_id) const {
    GetLogPage cmd{namespace_id, LogPageId::IntelVendorLatencyHistogramWrite};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_latency_log());
}


LogPageLatencyStats NvmeInterface::get_read_latency_histogram(const std::string& target,
                                                              std::uint32_t namespace_id) const {
    GetLogPage cmd{namespace_id, LogPageId::IntelVendorLatencyHistogramRead};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_latency_log());
}


ControllerData NvmeInterface::get_controller_info(const std::string& target, uint16_t controller_id) const {
    Identify cmd{controller_id, 0, IdentifyCns::Controller};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_controller_data());
}


NamespaceData NvmeInterface::get_namespace_info(const std::string& target, uint32_t namespace_id) const {
    Identify cmd{0, namespace_id, IdentifyCns::Namespace};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_namespace_data());
}


NamespaceIdList NvmeInterface::get_active_namespaces(const std::string& target) const {
    Identify cmd{0, 0, IdentifyCns::NamespaceIdList};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_namespace_id_list());
}


NamespaceIdList NvmeInterface::get_allocated_namespaces(const std::string& target) const {
    Identify cmd{0, 0, IdentifyCns::AllocatedNamespaceIdList};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_namespace_id_list());
}


ControllerIdList NvmeInterface::get_controllers(const std::string& target) const {
    Identify cmd{0, 0, IdentifyCns::ControllerIdList};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_controller_id_list());
}


ControllerIdList NvmeInterface::get_namespace_controllers(const std::string& target, uint32_t namespace_id) const {
    Identify cmd{0, namespace_id, IdentifyCns::NamespaceControllerIdList};
    m_invoker->execute(target, cmd);
    return std::move(cmd.get_controller_id_list());
}


void NvmeInterface::attach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const {
    NamespaceAttachment cmd{namespace_id, NamespaceAttachmentSel::Attach};
    cmd.add_controller(controller_id);
    m_invoker->execute(target, cmd);
}


void NvmeInterface::detach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const {
    NamespaceAttachment cmd{namespace_id, NamespaceAttachmentSel::Detach};
    cmd.add_controller(controller_id);
    m_invoker->execute(target, cmd);
}


void NvmeInterface::delete_namespace(const std::string& target, uint32_t namespace_id) const {
    NamespaceManagement cmd{NamespaceManagementSel::Delete};
    cmd.set_namespace_id(namespace_id);
    m_invoker->execute(target, cmd);
}


uint32_t NvmeInterface::create_namespace(const std::string& target,
                                         uint64_t size,
                                         uint64_t capacity,
                                         bool is_private) const {
    NamespaceManagement cmd{NamespaceManagementSel::Create};
    cmd.set_size(size);
    cmd.set_capacity(capacity);
    if (is_private) {
        cmd.set_as_private();
    }
    else {
        cmd.set_as_public();
    }
    m_invoker->execute(target, cmd);
    return cmd.get_namespace_id();
}


std::uint32_t NvmeInterface::get_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) {
    static const constexpr std::uint8_t INTEL_LATENCY_TRACKING = 0xe2;
    GetFeatures cmd{namespace_id, INTEL_LATENCY_TRACKING};
    m_invoker->execute(target, cmd);
    return cmd.get_data().result;
}


void NvmeInterface::enable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) {
    static const constexpr std::uint8_t INTEL_LATENCY_TRACKING = 0xe2;
    SetFeatures cmd{namespace_id, INTEL_LATENCY_TRACKING, 1};
    m_invoker->execute(target, cmd);
}


void NvmeInterface::disable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) {
    static const constexpr std::uint8_t INTEL_LATENCY_TRACKING = 0xe2;
    SetFeatures cmd{namespace_id, INTEL_LATENCY_TRACKING, 0};
    m_invoker->execute(target, cmd);
}
