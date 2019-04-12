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
 * @file nvme/nvme_interface.hpp
 */

#pragma once

#include "nvme/abstract_nvme_interface.hpp"
#include "nvme/nvme_invoker.hpp"

#include <string>

namespace nvme {

/*!
 * Implementation of the AbstractNvmeInterface
 */
class NvmeInterface : public AbstractNvmeInterface {
public:

    /*!
     * @brief Constructs a valid NvmeInterface, by default uses NvmeInvoker
     * @param invoker Invoker to be used by the interface, by default uses NvmeInvoker
     */
    NvmeInterface(std::shared_ptr<AbstractNvmeInvoker> invoker = std::make_shared<NvmeInvoker>());

    virtual ~NvmeInterface();

    /* Implementations of the AbstractNvmeInterface methods*/
    virtual void reset(const std::string& target) const override;
    virtual void flush(const std::string& target, uint32_t namespace_id) const override;
    virtual void format(const std::string& target, uint32_t namespace_id, FormatNvmSes format_type) const override;
    virtual LogPageSmart get_smart_log(const std::string& target, uint32_t namespace_id) const override;
    virtual LogPageFirmware get_firmware_log(const std::string& target, uint32_t namespace_id) const override;
    virtual LogPageIOQueues get_ioq_log(const std::string& target, std::uint32_t namespace_id) const override;
    virtual LogPageLatencyStats get_write_latency_histogram(const std::string& target, std::uint32_t namespace_id) const override;
    virtual LogPageLatencyStats get_read_latency_histogram(const std::string& target, std::uint32_t namespace_id) const override;
    virtual ControllerData get_controller_info(const std::string& target, uint16_t controller_id) const override;
    virtual NamespaceData get_namespace_info(const std::string& target, uint32_t namespace_id) const override;
    virtual NamespaceIdList get_active_namespaces(const std::string& target) const override;
    virtual NamespaceIdList get_allocated_namespaces(const std::string& target) const override;
    virtual ControllerIdList get_controllers(const std::string& target) const override;
    virtual ControllerIdList get_namespace_controllers(const std::string& target, uint32_t namespace_id) const override;
    virtual void attach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const override;
    virtual void detach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const override;
    virtual void delete_namespace(const std::string& target, uint32_t namespace_id) const override;
    virtual uint32_t create_namespace(const std::string& target, uint64_t size, uint64_t capacity, bool is_private) const override;
    virtual std::uint32_t get_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) override;
    virtual void enable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) override;
    virtual void disable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) override;

};

}
