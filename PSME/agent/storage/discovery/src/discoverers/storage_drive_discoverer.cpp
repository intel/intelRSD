/*!
 * @brief Drive discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file storage_drive_discoverer.cpp
 */

#include "drive_handling/base_drive_handler.hpp"
#include "discovery/discoverers/storage_drive_discoverer.hpp"
#include "discovery/builders/storage_drive_builder.hpp"

#include "agent/utils/utils.hpp"

#include <thread>



using namespace agent::storage;
using namespace agent::storage::discovery;

namespace {

void print_drive_data_log(const std::string& name, const BaseDriveHandler::DriveData& dd) {
    static const constexpr char DEFAULT_NULL[] = "null";

    log_debug("storage-agent", "Found drive: " << name << "\n\t" <<
        "Device type: " << dd.drive_type.value_or(DEFAULT_NULL) << "\n\t" <<
        "Model: " << dd.model_number.value_or(DEFAULT_NULL) << "\n\t" <<
        "FW Revision: " << dd.firmware_revision.value_or(DEFAULT_NULL) << "\n\t" <<
        "Manufacturer: " << dd.manufacturer_number.value_or(DEFAULT_NULL) << "\n\t" <<
        "Serial: " << dd.serial_number.value_or(DEFAULT_NULL) << "\n\t" <<
        "Total size blocks: " << dd.size_lba.value_or(0) << "\n\t" <<
        "Capacity: " << (dd.block_size_bytes.has_value() && dd.size_lba.has_value() ?
            std::uint64_t(dd.block_size_bytes) * std::uint64_t(dd.size_lba) : 0));
}

}


std::vector<agent_framework::model::Drive> StorageDriveDiscoverer::discover(const Uuid& parent_uuid) {

    std::vector<agent_framework::model::Drive> drives{};
    std::list<std::thread> threads{};
    std::mutex insertion_mutex{};

    log_debug("storage-agent", "Reading drives...");
    auto discover_device = [&](const std::string& drive_name) {
        log_debug("storage-agent", "Discovered drive " << drive_name);

        auto drive = StorageDriveBuilder::build_default(parent_uuid);
        auto handler = m_context->drive_handler_factory->get_handler(drive_name);

        drive.set_name(drive_name);
        if (!handler) {
            log_error("storage-agent", "No handler available for the drive " << drive_name);
            StorageDriveBuilder::update_critical_state(drive);
        }
        else {
            auto dd = handler->get_drive_data();
            StorageDriveBuilder::update_with_drive_handler(drive, dd);
            ::print_drive_data_log(drive_name, dd);
        }

        if (drive.get_fru_info().get_serial_number().has_value()) {
            std::lock_guard<std::mutex> guard(insertion_mutex);
            drives.emplace_back(std::move(drive));
        }
        else {
            log_warning("storage-agent", "Drive " << drive.get_name() << " has no serial number. Skipping...");
        }
    };

    for (const auto& drive_name : m_context->drive_reader->get_drives()) {
        threads.emplace_back(discover_device, drive_name);
    }

    // Join all threads
    std::for_each(std::begin(threads), std::end(threads), std::mem_fun_ref(&std::thread::join));
    return drives;
}

