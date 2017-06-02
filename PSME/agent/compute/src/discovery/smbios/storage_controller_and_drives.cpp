/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file storage_controller_and_drives.cpp
 * @brief SMBIOS-way of discovering drives on a computer system
 * */

#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "discovery/smbios/storage_controller_and_drives.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace {

std::string& rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
            s.end());
    return s;
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_drives(SmbiosParser::Ptr smbios, System& system,
                                                const std::string& chassis_uuid) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::OK);
    }

    attribute::Status status{};
    status.set_state(enums::State::Enabled);
    status.set_health(enums::Health::OK);

    // create virtual storage subsystem to hold the discovered drives
    StorageSubsystem storage{system.get_uuid()};
    storage.add_collection(attribute::Collection(
        enums::CollectionName::Drives,
        enums::CollectionType::Drives,
        ""
    ));
    storage.add_collection(attribute::Collection(
        enums::CollectionName::StorageControllers,
        enums::CollectionType::StorageControllers,
        ""
    ));
    storage.set_status(status);

    auto smbios_drives = smbios->get_all<SMBIOS_STORAGE_INFO_DATA>();
    for (const auto& drive_info : smbios_drives) {
        Drive drive{chassis_uuid};
        drive.set_capacity_gb(drive_info.data.RsaHddInfoData.HDDSizeInGB);

        std::string string_name(reinterpret_cast<const char*>(drive_info.data.RsaHddInfoData.HDDName),
                HDD_INFO_DATA::HDD_NAME_STRING_SIZE);

        attribute::FruInfo fru;
        fru.set_model_number(rtrim(string_name));
        drive.set_fru_info(std::move(fru));
        drive.set_status(status);

        // In the Drive model, these fields have default values.
        // We reset them to null optionals.
        drive.set_interface(OptionalField<enums::StorageProtocol>{});
        drive.set_type(OptionalField<enums::DriveType>{});

        CommonComponents::get_instance()->get_drive_manager().add_entry(std::move(drive));
        log_debug(GET_LOGGER("smbios-discovery"), "Drive was discovered.");
    }
    auto smbios_drives_v2 = smbios->get_all<SMBIOS_STORAGE_INFO_DATA_V2>();
    for (const auto& drive_info : smbios_drives_v2) {
        Drive drive{chassis_uuid};
        drive.set_capacity_gb(drive_info.data.RsaHddInfoData.HDDSizeInGB);

        std::string string_name(reinterpret_cast<const char*>(drive_info.data.RsaHddInfoData.HDDName),
                HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE);
        std::string serial_number(reinterpret_cast<const char*>(drive_info.data.RsaHddInfoData.HDDSerialNumber),
                HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE);

        attribute::FruInfo fru;
        fru.set_model_number(rtrim(string_name));
        fru.set_serial_number(rtrim(serial_number));
        drive.set_fru_info(std::move(fru));
        drive.set_status(status);

        // In the Drive model, these fields have default values.
        // We reset them to null optionals.
        drive.set_interface(OptionalField<enums::StorageProtocol>{});
        drive.set_type(OptionalField<enums::DriveType>{});

        CommonComponents::get_instance()->get_drive_manager().add_entry(std::move(drive));
        log_debug(GET_LOGGER("smbios-discovery"), "Drive V2 was discovered.");
    }

    // No create storage controllers are created in basic discovery because the agent
    // is unable to read any identifying data (model, serial, type) that could be used
    // to integrate the storage controller with the one read from deep discovery.
    CommonComponents::get_instance()->get_storage_subsystem_manager().add_entry(std::move(storage));

    log_info(GET_LOGGER("smbios-discovery"), "Storage Subsystem for SMBIOS drives created successfully.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
