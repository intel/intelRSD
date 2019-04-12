/*!
 * @brief System builder class implementation.
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
 * @file system_builder.cpp
 */

#include "discovery/builders/system_builder.hpp"
#include "smbios/utilities/conversions.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


System SystemBuilder::build_default(const std::string& parent_uuid, const std::string& chassis_uuid) {
    System system{parent_uuid};

    system.add_collection({enums::CollectionName::Processors, enums::CollectionType::Processors});
    system.add_collection({enums::CollectionName::Memory, enums::CollectionType::Memory});
    system.add_collection({enums::CollectionName::StorageSubsystems, enums::CollectionType::StorageSubsystems});
    system.add_collection({enums::CollectionName::NetworkInterfaces, enums::CollectionType::NetworkInterfaces});
    system.add_collection({enums::CollectionName::TrustedModules, enums::CollectionType::TrustedModules});
    system.add_collection({enums::CollectionName::NetworkDevices, enums::CollectionType::NetworkDevices});
    system.add_collection({enums::CollectionName::MemoryDomains, enums::CollectionType::MemoryDomains});

    system.set_chassis(chassis_uuid);

    system.set_status({enums::State::Enabled, {}});

    agent_framework::model::System::BootOverrideSupported boot_supported;
    boot_supported.add_entry(enums::BootOverrideTarget::Hdd);
    boot_supported.add_entry(enums::BootOverrideTarget::Pxe);
    boot_supported.add_entry(enums::BootOverrideTarget::None);
    boot_supported.add_entry(enums::BootOverrideTarget::RemoteDrive);
    system.set_boot_override_supported(boot_supported);

    return system;
}


void SystemBuilder::update_smbios_system_info(agent_framework::model::System& system,
                                              const smbios::parser::SmbiosParser::StructEnhanced <smbios::parser::SMBIOS_SYSTEM_INFO_DATA>& smbios_data) {

    const auto& system_guid = smbios::utilities::get_system_uuid(smbios_data.data);
    const auto& sku_number = smbios_data.get_string(smbios_data.data.sku_number);
    const auto& manufacturer = smbios_data.get_string(smbios_data.data.manufacturer);
    const auto& model = smbios_data.get_string(smbios_data.data.product_name);
    const auto& version = smbios_data.get_string(smbios_data.data.version);

    attribute::FruInfo fru_info = system.get_fru_info();
    fru_info.set_manufacturer(manufacturer);
    fru_info.set_model_number(model);
    fru_info.set_part_number(version);

    system.set_fru_info(fru_info);
    system.set_guid(system_guid);
    system.set_sku(sku_number);
}


void SystemBuilder::update_smbios_module_info(agent_framework::model::System& system,
                                              const smbios::parser::SmbiosParser::StructEnhanced <smbios::parser::SMBIOS_MODULE_INFO_DATA>& smbios_data) {
    const auto& serial_number = smbios_data.get_string(smbios_data.data.serial_number);

    attribute::FruInfo fru_info = system.get_fru_info();
    fru_info.set_serial_number(serial_number);

    system.set_fru_info(fru_info);
}


void SystemBuilder::update_smbios_bios_info(agent_framework::model::System& system,
                                            const smbios::parser::SmbiosParser::StructEnhanced <smbios::parser::SMBIOS_BIOS_INFO_DATA>& smbios_data) {
    const auto& bios_version = smbios_data.get_string(smbios_data.data.version);

    system.set_bios_version(bios_version);
}


void SystemBuilder::update_smbios_pcie_info(agent_framework::model::System& system,
                                            const std::vector <smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>>& smbios_data) {

    for (const auto& pci_device_info : smbios_data) {

        std::stringstream stream;
        stream << "0x" << std::hex << pci_device_info.data.VendorID;
        std::string vendor_id = stream.str();
        stream.str("");
        stream << "0x" << std::hex << pci_device_info.data.DeviceID;
        std::string device_id = stream.str();

        attribute::PciDevice pci_device(vendor_id, device_id);
        system.add_pci_device(pci_device);
    }
}


void SystemBuilder::update_smbios_txt_info(agent_framework::model::System& system, const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_TXT_INFO_DATA>& smbios_data) {
    bool is_txt_enabled = smbios::parser::TxtStatus::ENABLED == smbios_data.data.status ? true : false;
    system.set_txt_enabled(is_txt_enabled);
}


void SystemBuilder::update_system_guid(agent_framework::model::System& system,
                                       const ipmi::command::generic::response::GetSystemGuid& get_system_guid_response) {
    system.set_guid(get_system_guid_response.get_guid());
}

void SystemBuilder::update_smbios_performance_configurations(agent_framework::model::System& system,
                                                             const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>& smbios_data) {
    system.set_current_performance_configuration(smbios_data.data.current_config);

    for (const auto& config : smbios_data.configs) {
        attribute::PerformanceConfiguration performance_configuration;
        performance_configuration.set_configuration_id(config.configuration_number);
        performance_configuration.set_tdp(config.max_tdp);
        performance_configuration.set_max_junction_temp_celsius(config.max_junction_temperature);

        if (config.low_priority_core_count == 0) {
            performance_configuration.set_type(enums::PerformanceConfigurationType::StaticSpeedSelect);
            performance_configuration.set_active_cores(config.high_priority_core_count);
            performance_configuration.set_base_core_frequency(config.high_priority_base_frequency);
        }
        else {
            performance_configuration.set_type(enums::PerformanceConfigurationType::PrioritizedBaseFrequency);
            performance_configuration.set_high_priority_core_count(config.high_priority_core_count);
            performance_configuration.set_high_priority_base_frequency(config.high_priority_base_frequency);
            performance_configuration.set_low_priority_core_count(config.low_priority_core_count);
            performance_configuration.set_low_priority_base_frequency(config.low_priority_base_frequency);
        }

        system.add_performance_configuration(performance_configuration);
    }
}

