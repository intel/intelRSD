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
 * @file non_functional_drive_handler.cpp
 */

#include "agent-framework/module/utils/to_hex_string.hpp"
#include "drive_handling/non_functional_drive_handler.hpp"
#include "drive_handling/utils/nvme/tools.hpp"

#include "nvme/nvme_exception.hpp"
#include "logger/logger.hpp"

#include <cstdlib>
#include <sstream>

using namespace agent::storage;

namespace {

constexpr std::size_t MODEL_NUMBER_LENGTH = 40;
constexpr std::size_t SERIAL_NUMBER_LENGTH = 20;
constexpr std::size_t FIRMWARE_REVISION_LENGTH = 8;
constexpr std::uint8_t CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY = 0x04;

}

NonFunctionalDriveHandler::~NonFunctionalDriveHandler() {}

void NonFunctionalDriveHandler::load() {
    try {
        log_debug("drive-handler", "Loading drive data: " << m_name);
        // Non functional drive - only controller data may be read
        m_controller_data = m_nvme_interface->get_controller_info(m_name);
    }
    catch (const nvme::NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }
}

BaseDriveHandler::DriveData NonFunctionalDriveHandler::get_drive_data() const {
    // only controller data was read, other fields will be null
    DriveData dd{};
    dd.model_number = utils::convert_uint8_to_string(m_controller_data.model_number, ::MODEL_NUMBER_LENGTH);
    dd.serial_number = utils::convert_uint8_to_string(m_controller_data.serial_number, ::SERIAL_NUMBER_LENGTH);
    dd.firmware_revision = utils::convert_uint8_to_string(m_controller_data.firmware_revision, ::FIRMWARE_REVISION_LENGTH);
    dd.manufacturer_number = agent_framework::model::utils::to_hex_string<2>(m_controller_data.pci_vendor_id);
    dd.storage_protocol = BaseDriveHandler::NVME_PROTOCOL;
    dd.drive_type = BaseDriveHandler::SSD_TYPE;
    dd.namespaces = {};
    return dd;
}

BaseDriveHandler::SmartData NonFunctionalDriveHandler::get_smart_info() const {
    // smart cannot be read from the drive, it hardcoded in the way that drive is exposed with the critical health
    BaseDriveHandler::SmartData sm{};
    sm.critical_warnings = CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY;
    return sm;
}

