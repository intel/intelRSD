/*!
 * @brief Drive builder class implementation.
 *
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
 * @file storage_drive_builder.cpp
 */

#include "discovery/builders/storage_drive_builder.hpp"



using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


void StorageDriveBuilder::update_with_drive_handler(agent_framework::model::Drive& drive,
                                                    const agent::storage::BaseDriveHandler::DriveData& drive_data) {

    if (drive_data.block_size_bytes.has_value()) {
        drive.set_block_size_bytes(drive_data.block_size_bytes);
        if (drive_data.size_lba.has_value()) {
            drive.set_capacity_gb(double(drive_data.size_lba * drive_data.block_size_bytes) / 1000.0 / 1000.0 / 1000.0);
        }
    }

    if (drive_data.drive_type.has_value()) {
        drive.set_type(model::enums::DriveType::from_string(drive_data.drive_type.value()));
    }

    if (drive_data.rpm.has_value()) {
        drive.set_rpm(drive_data.rpm.value());
    }

    model::attribute::FruInfo fru_info{};
    if (drive_data.serial_number.has_value()) {
        fru_info.set_serial_number(drive_data.serial_number);
    }
    if (drive_data.model_number.has_value()) {
        fru_info.set_model_number(drive_data.model_number);
    }
    if (drive_data.manufacturer_number.has_value()) {
        fru_info.set_manufacturer(drive_data.manufacturer_number);
    }
    drive.set_fru_info(std::move(fru_info));

    if (drive_data.firmware_revision.has_value()) {
        drive.set_firmware_version(drive_data.firmware_revision);
    }

    if (drive_data.storage_protocol.has_value()) {
        drive.set_interface(model::enums::TransportProtocol::from_string(drive_data.storage_protocol));
    }
}

