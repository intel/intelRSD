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
 * */

#include "agent-framework/command/storage/get_physical_drive_info.hpp"
#include "agent-framework/module/module_manager.hpp"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::generic;

/*! GetPhysicalDriveInfo implementation */
class GetPhysicalDriveInfo : public storage::GetPhysicalDriveInfo {
public:
    GetPhysicalDriveInfo() {}

    using storage::GetPhysicalDriveInfo::execute;

    void execute(const Request& request, Response& response) {
        auto drive = request.get_drive();

        auto hard_drive = ModuleManager::find_hard_drive(drive).lock();
        if (!hard_drive) {
            THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                  "Drive drive = '" + drive + "' not found.");
        }


        PhysicalDrive physical_drive;

        // TODO hardcoded
        physical_drive.set_interface(enums::PhysicalDriveInterface::SATA);

        physical_drive.set_status(attribute::Status(
                           hard_drive->get_status().get_state(),
                           hard_drive->get_status().get_health()));
        physical_drive.set_type(enums::PhysicalDriveType::from_string(hard_drive->get_type()));
        physical_drive.set_capacity_gb(hard_drive->get_capacity_gb());
        if(hard_drive->get_rpm() > 0) {
            physical_drive.set_rpm(hard_drive->get_rpm());
        }
        attribute::FruInfo fru{};
        fru.set_model_number(hard_drive->get_fru_info().get_model_number());
        fru.set_serial_number(hard_drive->get_fru_info().get_serial_number());
        if (!hard_drive->get_fru_info().get_manufacturer().empty())
            hard_drive->get_fru_info().get_manufacturer();
        fru.set_part_number(hard_drive->get_fru_info().get_part_number());
        physical_drive.set_fru_info(fru);
        response.set_drive(physical_drive);
    }

    ~GetPhysicalDriveInfo();
};

GetPhysicalDriveInfo::~GetPhysicalDriveInfo() {}


static Command::Register<GetPhysicalDriveInfo> g("ConfigurationBased");
