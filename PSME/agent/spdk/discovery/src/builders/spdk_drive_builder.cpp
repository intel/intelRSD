/*!
 * @brief Drive builder class implementation.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file drive_builder.cpp
 */

#include "discovery/builders/spdk_drive_builder.hpp"



using namespace agent_framework;
using namespace agent::spdk::discovery;

namespace {

static constexpr const char DRIVE_DESCRIPTION[] = "SPDK NVMe Block Device (NVMe BDEV)";

}


void SpdkDriveBuilder::update_with_spdk(model::Drive& drive, const ::spdk::model::Bdev& bdev) {

    drive.set_parent_type(model::enums::Component::Chassis);

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    drive.set_status(std::move(status));

    drive.set_block_size_bytes(bdev.get_block_size());
    drive.set_name(bdev.get_name());
    drive.set_description(::DRIVE_DESCRIPTION);

    drive.set_interface(model::enums::TransportProtocol::NVMe);
    drive.set_type(model::enums::DriveType::SSD);

    auto capacity_bytes = bdev.get_num_blocks() * bdev.get_block_size();
    drive.set_capacity_gb((double) capacity_bytes / (1000.0 * 1000.0 * 1000.0));

    auto nvme_driver = bdev.get_driver_specific<::spdk::model::drivers::NvmeDriver>();
    if (nvme_driver) {
        ::spdk::model::ControllerData ctrl_data = nvme_driver->get_ctrlr_data();
        drive.set_firmware_version(ctrl_data.get_firmware_revision());

        model::attribute::FruInfo fru_info{};
        fru_info.set_serial_number(ctrl_data.get_serial_number());
        fru_info.set_model_number(ctrl_data.get_model_number());
        fru_info.set_manufacturer(ctrl_data.get_vendor_id());
        drive.set_fru_info(std::move(fru_info));
    }
}