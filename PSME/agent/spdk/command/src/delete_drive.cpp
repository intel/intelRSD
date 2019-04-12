/*!
 * @brief Implementation of DeleteDrive command.
 *
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
 * @file delete_drive.cpp
 */
#include "spdk_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::spdk;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void delete_drive(DeleteDrive::ContextPtr, const DeleteDrive::Request& request,
                  DeleteDrive::Response&) {

    log_info("spdk-agent", "Deleting drive...");
    auto drive_uuid = request.get_drive();
    auto drive = get_manager<Drive>().get_entry(drive_uuid);

    if (drive.get_status().get_state() == enums::State::Absent) {

        for (const auto& storage_service_uuid: get_m2m_manager<StorageService, Drive>().get_parents(drive_uuid)) {
            get_m2m_manager<StorageService, Drive>().remove_entry(storage_service_uuid, drive_uuid);
        }
        get_manager<Drive>().remove_entry(drive_uuid);

        log_info("spdk-agent", "Drive " << drive_uuid << " removed successfully");
    }
    else {
        THROW(exceptions::InvalidValue, "spdk-agent", "Cannot delete Drive if its state is not 'absent'.");
    }

}
}
REGISTER_SPDK_COMMAND(DeleteDrive, ::delete_drive);