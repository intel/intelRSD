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
 * @file nvme/abstract_nvme_interface.hpp
 */

#pragma once

#include "nvme/abstract_nvme_invoker.hpp"

#include <string>
#include <memory>

namespace nvme {

/*!
 * Nvme access interface, external interface for querying NVMe commands, fasade for all other classes
 */
class AbstractNvmeInterface {
public:

    /*!
     * @brief Constructs a valid NvmeInterface
     * @param invoker Invoker to be used by the interface
     */
    AbstractNvmeInterface(std::shared_ptr<AbstractNvmeInvoker> invoker): m_invoker(invoker) {}

    virtual ~AbstractNvmeInterface();

    /*!
     * @brief Sets the NVMe invoker
     * @param invoker New invoker to be used by the interface
     */
    virtual void set_invoker(std::shared_ptr<AbstractNvmeInvoker> invoker) {
        m_invoker = invoker;
    }

    /*!
     * @brief Reset NVMe controller
     * @param target Target device
     */
    virtual void reset(const std::string& target) const = 0;

    /*!
     * @brief Flushes NVMe namespace
     * @param target Target device
     * @param namespace_id Id of the target namespace
     */
    virtual void flush(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Formats NVMe namespace
     * @param target Target device
     * @param namespace_id Id of the target namespace
     * @param format_type Type of the format to be performed
     */
    virtual void format(const std::string& target, uint32_t namespace_id, FormatNvmSes format_type) const = 0;

    /*!
     * @brief Returns the smart log for the namespace
     * @param target Target device
     * @param namespace_id Id of the namespace
     * @return Smart log
     */
    virtual LogPageSmart get_smart_log(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Returns the firmware log for the namespace
     * @param target Target device
     * @param namespace_id Id of the namespace
     * @return Firmware log
     */
    virtual LogPageFirmware get_firmware_log(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Returns the IO queues metrics log for the requested namespace.
     * @param target Target device.
     * @param namespace_id ID of the namespace.
     * @return IO queues log
     */
    virtual LogPageIOQueues get_ioq_log(const std::string& target, std::uint32_t namespace_id) const = 0;

    /*!
     * @brief Get the write latency histogram log page for the namespace.
     * @param target Target device.
     * @param namespace_id ID of the namespace.
     * @return Write latency log.
     */
    virtual LogPageLatencyStats get_write_latency_histogram(const std::string& target, std::uint32_t namespace_id) const = 0;

    /*!
     * @brief Get the read latency histogram log page for the namespace.
     * @param target Target device.
     * @param namespace_id ID of the namespace.
     * @return Read latency log.
     */
    virtual LogPageLatencyStats get_read_latency_histogram(const std::string& target, std::uint32_t namespace_id) const = 0;

    /*!
     * @brief Identifies the NVMe controller
     * @param target Target device
     * @param controller_id Id of the controller, 0 by default
     * @return Controller data
     */
    virtual ControllerData get_controller_info(const std::string& target, uint16_t controller_id = 0) const = 0;

    /*!
     * @brief Identifies the NVMe namespace
     * @param target Target device
     * @param namespace_id Id of the namespace
     * @return Namespace data
     */
    virtual NamespaceData get_namespace_info(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Identifies active namespaces
     * @param target Target device
     * @return List of active namespaces
     */
    virtual NamespaceIdList get_active_namespaces(const std::string& target) const = 0;

    /*!
     * @brief Identifies allocated namespaces
     * @param target Target device
     * @return List of allocated namespaces
     */
    virtual NamespaceIdList get_allocated_namespaces(const std::string& target) const = 0;

    /*!
     * @brief Identifies controllers
     * @param target Target device
     * @return List of all controllers
     */
    virtual ControllerIdList get_controllers(const std::string& target) const = 0;

    /*!
     * @brief Identifies controllers attached to the specified namespace
     * @param target Target device
     * @param namespace_id Queried namespace id
     * @return List of attached controllers
     */
    virtual ControllerIdList get_namespace_controllers(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Attaches controller to the namespace
     * @param target Target device
     * @param controller_id Id of the controller to be attached
     * @param namespace_id Namespace id
     */
    virtual void attach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const = 0;

    /*!
     * @brief Detaches controller from the namespace
     * @param target Target device
     * @param controller_id Id of the controller to be detached
     * @param namespace_id Namespace id
     */
    virtual void detach_namespace(const std::string& target, uint16_t controller_id, uint32_t namespace_id) const = 0;

    /*!
     * @brief Deletes a namespace
     * @param target Target device
     * @param namespace_id Id of the namespace to be deleted
     */
    virtual void delete_namespace(const std::string& target, uint32_t namespace_id) const = 0;

    /*!
     * @brief Creates a namespace
     * @param target Target device
     * @param size Size of the namespace
     * @param capacity Capacity of the namespace
     * @param is_private Is it a private (single controller) namespace
     * @return Created namespace id
     */
    virtual uint32_t create_namespace(const std::string& target, uint64_t size, uint64_t capacity, bool is_private) const = 0;


    /*!
     * @brief Check if device supports latency tracking.
     * @param[in] target Target device.
     * @param[in] namespace_id Namespace ID.
     * @return Returns 1 if device supports latency tracking, 0 otherwise.
     */
    virtual std::uint32_t get_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) = 0;


    /*!
     * @brief Request enabling latency tracking feature.
     * @param[in] target Target device.
     * @param[in] namespace_id Namespace ID.
     */
    virtual void enable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) = 0;


    /*!
     * @brief Request disabling latency tracking feature.
     * @param[in] target Target device.
     * @param[in] namespace_id Namespace ID.
     */
    virtual void disable_latency_tracking_feature(const std::string& target, std::uint32_t namespace_id) = 0;


protected:
    std::shared_ptr<AbstractNvmeInvoker> m_invoker{};

};

}
