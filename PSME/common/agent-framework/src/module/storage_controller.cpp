/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/storage_controller.hpp"
#include "json/json.hpp"

using namespace agent_framework::generic;

void StorageController::read_configuration(const json::Value& controller_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading storage controller configuration.");
        m_interface = controller_configuration["interface"].as_string();
        m_drive_count = controller_configuration["driveCount"].as_uint();
        m_fru_info.set_serial_number(controller_configuration["fruInfo"]["serialNumber"].as_string());
        m_fru_info.set_manufacturer(controller_configuration["fruInfo"]["manufacturer"].as_string());
        m_fru_info.set_model_number(controller_configuration["fruInfo"]["modelNumber"].as_string());
        m_fru_info.set_part_number(controller_configuration["fruInfo"]["partNumber"].as_string());
        set_status({controller_configuration["status"]["state"].as_string(),
                    controller_configuration["status"]["health"].as_string()});

        // Read hard disk drives configuration
        for (const auto& drive_json : controller_configuration["drives"]) {
            auto drive = std::make_shared<HardDrive>();
            drive->read_configuration(drive_json);
            add_hard_drive(std::move(drive));
        }
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing storage controller configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in storageControllers section");
    }
}


StorageController::~StorageController(){}

HardDriveWeakPtr StorageController::find_hard_drive(const std::string& uuid) const {
    const auto& hard_drives = get_hard_drives();
    for (const auto& hard_drive : hard_drives) {
        if (hard_drive->get_uuid() == uuid) {
            return hard_drive;
        }
    }
    return {};
}
