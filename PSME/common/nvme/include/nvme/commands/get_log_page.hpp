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
 * @file nvme/commands/get_log_page.hpp
 */

#pragma once

#include "nvme/commands/generic_nvme_command.hpp"

namespace nvme {
namespace commands {

class GetLogPage final : public GenericNvmeCommand {
public:

    static constexpr uint16_t BUFFER_SIZE = 1024;

    /*!
     * @brief Constructs a valid GetLogPage command
     * @param namespace_id Id of the affected namespace
     * @param log_page_id Id of the log page
     */
    GetLogPage(uint32_t namespace_id, LogPageId log_page_id);

    virtual ~GetLogPage();

    /*!
     * @brief Returns log interpreted as smart log
     * @return Smart log
     */
    const LogPageSmart& get_smart_log() const {
        return m_log_smart;
    }

    /*!
     * @brief Returns log interpreted as smart log
     * @return Smart log
     */
    const LogPageFirmware& get_firmware_log() const {
        return m_log_firmware;
    }

    /*!
     * @brief Returns log interpreted as latency histogram
     * @return Latency log
     */
    const LogPageLatencyStats& get_latency_log() const {
        return m_log_latency;
    }

    /*!
     * @brief Returns log interpreted as metrics of IO submission and completion queues.
     * @return IOQs log
     */
    const LogPageIOQueues& get_io_queues_log() const {
        return m_log_io_queues;
    }

private:

    union {
        LogPageSmart m_log_smart;
        LogPageFirmware m_log_firmware;
        LogPageLatencyStats m_log_latency;
        LogPageIOQueues m_log_io_queues;
        uint8_t m_raw_buffer[BUFFER_SIZE]{};
    };

};

}
}
