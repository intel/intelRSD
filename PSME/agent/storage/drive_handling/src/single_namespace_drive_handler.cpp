/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file single_namespace_drive_handler.cpp
 */

#include "agent-framework/module/utils/to_hex_string.hpp"

#include "drive_handling/single_namespace_drive_handler.hpp"
#include "drive_handling/utils/nvme/tools.hpp"

#include "logger/logger.hpp"
#include "nvme/nvme_exception.hpp"
#include "nvme/utils.hpp"

#include <cstdlib>

using namespace agent::storage;

namespace {

constexpr unsigned ERASE_BYTES_PER_SINGLE_WRITE = 10 * 1024 * 1024; // Bytes to write during single write operation

constexpr std::size_t MODEL_NUMBER_LENGTH = 40;
constexpr std::size_t SERIAL_NUMBER_LENGTH = 20;
constexpr std::size_t FIRMWARE_REVISION_LENGTH = 8;

}

SingleNamespaceDriveHandler::~SingleNamespaceDriveHandler() {}

void SingleNamespaceDriveHandler::load() {
    std::lock_guard<std::mutex> lock{m_mutex};

    static constexpr std::uint32_t MAX_LBA = 0xFFFFFFFF;

    try {
        log_debug("drive-handler", "Loading drive data: " << m_name);

        // get NVMe drive data
        m_namespace_id = get_primary_namespace_id();
        m_controller_data = m_nvme_interface->get_controller_info(m_name);
        m_namespace_data = m_nvme_interface->get_namespace_info(m_name, m_namespace_id);
        std::uint64_t lba_size = nvme::get_formatted_lba_data_size(m_namespace_data);
        std::string device_name = utils::get_namespace_device_name(m_name, m_namespace_id);
        if (lba_size > MAX_LBA) {
            throw std::runtime_error(std::string{"Unsupported LBA size: "} + std::to_string(lba_size));
        }
        m_lba_size = std::uint32_t(lba_size);
    }
    catch (const nvme::NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }
}

SingleNamespaceDriveHandler::DriveData SingleNamespaceDriveHandler::get_drive_data() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    DriveData dd{};
    dd.block_size_bytes = m_lba_size;
    dd.size_lba = m_namespace_data.size;
    dd.model_number = utils::convert_uint8_to_string(m_controller_data.model_number, ::MODEL_NUMBER_LENGTH);
    dd.serial_number = utils::convert_uint8_to_string(m_controller_data.serial_number, ::SERIAL_NUMBER_LENGTH);
    dd.firmware_revision = utils::convert_uint8_to_string(m_controller_data.firmware_revision, ::FIRMWARE_REVISION_LENGTH);
    dd.manufacturer_number = agent_framework::model::utils::to_hex_string<2>(m_controller_data.pci_vendor_id);
    dd.storage_protocol = BaseDriveHandler::NVME_PROTOCOL;
    dd.drive_type = BaseDriveHandler::SSD_TYPE;
    dd.namespaces = { utils::get_namespace_device_name(m_name, m_namespace_id) };
    return dd;
}

std::uint32_t SingleNamespaceDriveHandler::get_primary_namespace_id() const {
    return nvme::find_first_namespace(m_nvme_interface->get_active_namespaces(m_name));
}

SingleNamespaceDriveHandler::SmartData SingleNamespaceDriveHandler::get_smart_info() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    SingleNamespaceDriveHandler::SmartData sm{};

    try {
        auto smart_log = m_nvme_interface->get_smart_log(m_name, SingleNamespaceDriveHandler::GLOBAL_LOG_PAGE_ID);

        sm.available_spare = smart_log.available_spare;
        sm.available_spare_threshold = smart_log.available_spare_threshold;
        sm.composite_temperature = smart_log.composite_temperature;
        sm.controller_busy_time = utils::convert_uint128_to_long_double((smart_log.controller_busy_time));
        sm.critical_warnings = smart_log.critical_warnings;
        sm.data_unit_read = utils::convert_uint128_to_long_double((smart_log.data_unit_read));
        sm.data_unit_written = utils::convert_uint128_to_long_double((smart_log.data_unit_written));
        sm.host_read_commands = utils::convert_uint128_to_long_double((smart_log.host_read_commands));
        sm.host_write_commands = utils::convert_uint128_to_long_double((smart_log.host_write_commands));
        sm.media_data_integrity_errors = utils::convert_uint128_to_long_double((smart_log.media_data_integrity_errors));
        sm.number_of_error_log_entries = utils::convert_uint128_to_long_double((smart_log.number_of_error_log_entries));
        sm.percentage_used = smart_log.percentage_used;
        sm.power_cycles = utils::convert_uint128_to_long_double((smart_log.power_cycles));
        sm.power_on_hours = utils::convert_uint128_to_long_double((smart_log.power_on_hours));
        sm.unsafe_shutdowns = utils::convert_uint128_to_long_double((smart_log.unsafe_shutdowns));
    }
    catch (const nvme::NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }

    return sm;
}
