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

#include "tools/drive_discovery/non_functional_drive_handler.hpp"
#include "nvme/nvme_exception.hpp"
#include "nvme/utils.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

#include <regex>
#include <chrono>
#include <random>
#include <stdlib.h>

using namespace agent::nvme::tools;
using namespace nvme;
using namespace partition;
using namespace agent_framework::model::utils;

namespace {

constexpr size_t MODEL_NUMBER_LENGTH = 40;
constexpr size_t SERIAL_NUMBER_LENGTH = 20;
constexpr size_t FIRMWARE_REVISION_LENGTH = 8;
constexpr uint8_t CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY = 0x04;

std::string convert_uint8_to_string(const uint8_t* array, size_t size) {
    std::stringstream ss{};
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == '\0') {
            break;
        }
        ss << array[i];
    }
    std::string s = ss.str();
    std::size_t first = s.find_first_not_of(" ");
    std::size_t last = s.find_last_not_of(" ");
    if (std::string::npos == first || std::string::npos == last || first > last) {
        return "";
    }
    return s.substr(first, last - first + 1);
}

}

NonFunctionalDriveHandler::~NonFunctionalDriveHandler() {}

void NonFunctionalDriveHandler::load() {
    try {
        log_debug("drive-handler", "Loading drive data: " << m_name);
        // non functional drive - only controller data may be read
        m_controller_data = m_nvme_interface->get_controller_info(m_name);
    }
    catch (const NvmeException& e) {
        log_error("drive-handler", "NVMe exception while reading drive data (" << m_name << "): " << e.what());
    }
    catch (const std::exception& e) {
        log_error("drive-handler", "Exception while reading drive data (" << m_name << "): " << e.what());
    }
}

NonFunctionalDriveHandler::DriveData NonFunctionalDriveHandler::get_drive_data() const {
    // only controller data was read, other fields will be null
    DriveData dd{};
    dd.model_number = convert_uint8_to_string(m_controller_data.model_number, ::MODEL_NUMBER_LENGTH);
    dd.serial_number = convert_uint8_to_string(m_controller_data.serial_number, ::SERIAL_NUMBER_LENGTH);
    dd.firmware_revision = convert_uint8_to_string(m_controller_data.firmware_revision, ::FIRMWARE_REVISION_LENGTH);
    dd.manufacturer_number = to_hex_string<2>(m_controller_data.pci_vendor_id);
    dd.namespaces = {};
    return dd;
}

std::vector<std::string> NonFunctionalDriveHandler::get_volumes() const {
    // return no volumes
    return {};
}

[[ noreturn ]] std::string NonFunctionalDriveHandler::create_volume(const uint64_t) const {
    // throw as this cannot be accomplished
    throw std::runtime_error("Unable to create volume: drive is not functional");
}

[[ noreturn ]] void NonFunctionalDriveHandler::remove_volume(const std::string&) const {
    // throw as this cannot be accomplished
    throw std::runtime_error("Unable to remove volume: drive is not functional");
}

[[ noreturn ]] NonFunctionalDriveHandler::VolumeData NonFunctionalDriveHandler::get_volume_data(const std::string&) const {
    // throw as this cannot be accomplished
    throw std::runtime_error("Unable to get volume data: drive is not functional");
}

NonFunctionalDriveHandler::SmartData NonFunctionalDriveHandler::load_smart_info() const {
    // smart cannot be read from the drive, it hardcoded in the way that drive is exposed with the critical health
    NonFunctionalDriveHandler::SmartData ret{};
    ret.critical_warnings = CRITICAL_WARNING_FLAG_DEGRADED_RELIABILITY;
    return ret;
}

NonFunctionalDriveHandler::LatencyData NonFunctionalDriveHandler::load_latency_histogram() const {
    BaseDriveHandler::LatencyData histogram{};
    return histogram;
}

[[ noreturn ]] void NonFunctionalDriveHandler::initialize_drive() {
    // throw as this cannot be accomplished
    throw std::runtime_error("Unable to initialize drive: drive is not functional");
}

[[ noreturn ]] void NonFunctionalDriveHandler::erase_volume(const std::string&, bool) {
    // throw as this cannot be accomplished
    throw std::runtime_error("Unable to erase volume: drive is not functional");
}
