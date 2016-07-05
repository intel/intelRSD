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

#include "agent-framework/module/hard_drive.hpp"

using namespace agent_framework::generic;

namespace {
constexpr const char* HARD_DRIVE_TYPE = "PhysicalDrive";
}

HardDrive::HardDrive() : BlockDevice(HARD_DRIVE_TYPE) {}

void HardDrive::read_configuration(const json::Value& drive_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading hard disk drive configuration.");
        m_interface = drive_configuration["interface"].as_string();
        m_type = drive_configuration["type"].as_string();
        m_capacity_gb = drive_configuration["capacityGB"].as_double();
        m_rpm = drive_configuration["rpm"].as_uint();
        m_fru_info.set_serial_number(drive_configuration["fruInfo"]["serialNumber"].as_string());
        m_fru_info.set_manufacturer(drive_configuration["fruInfo"]["manufacturer"].as_string());
        m_fru_info.set_model_number(drive_configuration["fruInfo"]["modelNumber"].as_string());
        m_fru_info.set_part_number(drive_configuration["fruInfo"]["partNumber"].as_string());
        set_status(Status(
            drive_configuration["status"]["state"].as_string(),
            drive_configuration["status"]["health"].as_string()));
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing hard disk drives configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in drives section");
    }
}

HardDrive::~HardDrive(){}
