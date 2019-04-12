/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @section Implementation of the discovery utility functions
 *
 * @file pnc/src/discovery/discovery_utils.cpp
 *
 * */

#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/model/zone.hpp"
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"

using namespace agent::pnc::gas;
using namespace agent::pnc::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace pnc {
namespace discovery {
namespace utils {

enums::Health partition_state_to_status_health(const PartitionState& ps) {
    switch (ps) {
        case PartitionState::ACTIVE:
            return enums::Health::OK;
        case PartitionState::UNINITIALIZED:
            return enums::Health::Critical;
        case PartitionState::FUNDAMENTAL_RESET_CLEAN_TLP_DONE:
        case PartitionState::FUNDAMENTAL_RESET_PSC:
        case PartitionState::FUNDAMENTAL_RESET_PTD:
        case PartitionState::HOT_RESET_CLEANUP_TLP_DONE:
        case PartitionState::HOT_RESET_PSC:
        case PartitionState::HOT_RESET_PTD:
        case PartitionState::USP_SECOND_BUS_RESET:
        default:
            return enums::Health::Warning;
    }
}

enums::State partition_status_to_status_state(const PartitionStatus& ps) {
    switch (ps) {
        case PartitionStatus::ACTIVE:
            return enums::State::Enabled;
        case PartitionStatus::INACTIVE:
            return enums::State::Disabled;
        default:
            return enums::State::Absent;
    }
}

agent_framework::model::attribute::Status state_operation_result_to_status(uint32_t so) {
    attribute::Status status{};
    switch (static_cast<OperationResult>(so)) {
    case OperationResult::SUCCESS:
        status.set_state(enums::State::Enabled);
        status.set_health(enums::Health::OK);
        break;
    case OperationResult::IN_PROGRESS:
        status.set_state(enums::State::Starting);
        status.set_health(enums::Health::OK);
        break;
    case OperationResult::FAILED :
        status.set_state(enums::State::StandbyOffline);
        status.set_health(enums::Health::Warning);
        break;
    case OperationResult::N_A :
    default:
        status.set_state(enums::State::Absent);
        status.set_health(enums::Health::Critical);
        break;
    }
    return status;
}

enums::PcieFunctionDeviceClass to_device_class_enum(uint8_t dc) {
    switch (dc) {
        case 0x00: return enums::PcieFunctionDeviceClass::UnclassifiedDevice;
        case 0x01: return enums::PcieFunctionDeviceClass::MassStorageController;
        case 0x02: return enums::PcieFunctionDeviceClass::NetworkController;
        case 0x03: return enums::PcieFunctionDeviceClass::DisplayController;
        case 0x04: return enums::PcieFunctionDeviceClass::MultimediaController;
        case 0x05: return enums::PcieFunctionDeviceClass::MemoryController;
        case 0x06: return enums::PcieFunctionDeviceClass::Bridge;
        case 0x07: return enums::PcieFunctionDeviceClass::CommunicationController;
        case 0x08: return enums::PcieFunctionDeviceClass::GenericSystemPeripheral;
        case 0x09: return enums::PcieFunctionDeviceClass::InputDeviceController;
        case 0x0A: return enums::PcieFunctionDeviceClass::DockingStation;
        case 0x0B: return enums::PcieFunctionDeviceClass::Processor;
        case 0x0C: return enums::PcieFunctionDeviceClass::SerialBusController;
        case 0x0D: return enums::PcieFunctionDeviceClass::WirelessController;
        case 0x0E: return enums::PcieFunctionDeviceClass::IntelligentController;
        case 0x0F: return enums::PcieFunctionDeviceClass::SatelliteCommunicationsController;
        case 0x10: return enums::PcieFunctionDeviceClass::EncryptionController;
        case 0x11: return enums::PcieFunctionDeviceClass::SignalProcessingController;
        case 0x12: return enums::PcieFunctionDeviceClass::ProcessingAccelerators;
        case 0x13: return enums::PcieFunctionDeviceClass::NonEssentialInstrumentation;
        case 0x40: return enums::PcieFunctionDeviceClass::Coprocessor;
        case 0xFF:
        default:
            return enums::PcieFunctionDeviceClass::UnassignedClass;
    }
}

std::string strip_spaces(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return std::string{};
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

std::string get_null_terminated_string(const std::uint8_t* data, const std::uint32_t size) {
    if (nullptr == data) {
        return std::string{};
    }
    std::string str{};
    if (data != nullptr && size > 0) {
        for (size_t i = 0; i < size; ++i) {
            if (data[i] != '\0') {
                str += data[i];
            }
        }
    }
    return strip_spaces(str);
}

OptionalField<std::string> get_null_terminated_optional_string(const std::uint8_t* data, const std::uint32_t size) {
    std::string str = get_null_terminated_string(data, size);
    if (str.empty()) {
        return OptionalField<std::string>();
    }
    return OptionalField<std::string>(str);
}

enums::ChassisType fru_chassis_type_to_model_chassis_type(fru_eeprom::parser::SystemEnclosureOrChassisType fru_type) {
    switch (fru_type) {
        case fru_eeprom::parser::SystemEnclosureOrChassisType::SUB_NOTEBOOK:
            return enums::ChassisType::Module;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::MAIN_SERVER_CHASSIS:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::MULTI_SYSTEM_CHASSIS:
            return enums::ChassisType::Drawer;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::MINI_TOWER:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::TOWER:
            return enums::ChassisType::Rack;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::SUB_CHASSIS:
            return enums::ChassisType::Blade;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::EXPANSION_CHASSIS:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::BUS_EXPANSION_CHASSIS:
            return enums::ChassisType::Expansion;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::RACK_MOUNT_CHASSIS:
            return enums::ChassisType::Enclosure;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::LOW_PROFILE_DESKTOP:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::NOTEBOOK:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::LAPTOP:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::DESKTOP:
            return enums::ChassisType::StandAlone;
        case fru_eeprom::parser::SystemEnclosureOrChassisType::ALL_IN_ONE:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::COMPACT_PCI:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::PERIPHERAL_CHASSIS:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::PORTABLE:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::RAID_CHASSIS:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::SEALED_CASE_PC:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::SPACE_SAVING:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::DOCKING_STATION:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::HANDHELD:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::LUNCH_BOX:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::PIZZA_BOX:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::UNKNOWN:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::ADVANCED_TCA:
        case fru_eeprom::parser::SystemEnclosureOrChassisType::OTHER:
        default:
            return enums::ChassisType::Other;
    }
}

void update_endpoint_zone_binding_from_db(const std::vector<Uuid>& zones_uuids,
                                          const Uuid& endpoint_uuid) {

    for (const auto& zone_uuid : zones_uuids) {

        if (!get_manager<Zone>().entry_exists(zone_uuid)) {

            log_error("pnc-discovery", "No zone found in manager with given UUID '" + zone_uuid + "'");
            return;
        }

        agent_framework::database::ZoneEntity db{zone_uuid};

        auto endpoints_uuids{db.get_multiple_values(agent::pnc::tools::db_keys::ENDPOINTS)};

        auto it = std::find(endpoints_uuids.begin(), endpoints_uuids.end(), endpoint_uuid);

        if (it != endpoints_uuids.end()) {

            log_debug("pnc-discovery", "Added db endpoint: " + *it + " to zone: " + zone_uuid);

            get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, *it);

            break;
        }
    }
}

}
}
}
}
