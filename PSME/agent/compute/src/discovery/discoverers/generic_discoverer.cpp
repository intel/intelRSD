/*!
 * @brief Compute agent generic discoverer implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file generic_discoverer.cpp
 */

#include "discovery/discoverers/generic_discoverer.hpp"
#include "discovery/discoverers/platform_specific/purley_discoverer.hpp"
#include "discovery/builders/processor_builder.hpp"
#include "discovery/builders/memory_builder.hpp"
#include "discovery/builders/memory_domain_builder.hpp"
#include "discovery/builders/memory_chunks_builder.hpp"
#include "discovery/builders/drive_builder.hpp"
#include "discovery/builders/network_interface_builder.hpp"
#include "discovery/builders/network_device_function_builder.hpp"
#include "discovery/builders/system_builder.hpp"
#include "discovery/builders/chassis_builder.hpp"
#include "discovery/builders/trusted_module_builder.hpp"
#include "discovery/builders/manager_builder.hpp"
#include "discovery/builders/pcie_device_builder.hpp"
#include "discovery/builders/pcie_function_builder.hpp"
#include "discovery/helpers/memory_helper.hpp"
#include "smbios/utilities/conversions.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "iscsi/structs/iscsi_mdr_attempt.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

#include "ipmi/command/generic/get_device_guid.hpp"
#include "ipmi/command/generic/get_device_id.hpp"


using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace smbios::parser;
using namespace ipmi::command;


GenericDiscoverer::~GenericDiscoverer() {}


void GenericDiscoverer::read_mdr() {
    log_debug("compute-discovery", "Trying to read MDR region");
    try {
        auto smbios_mdr_accessor = m_mdr_accessor_factory->create(get_platform_id(),
                                                                  m_management_controller,
                                                                  ipmi::command::sdv::DataRegionId::SMBIOS_TABLE);
        set_up_smbios_data(smbios_mdr_accessor->get_mdr_region());
        log_debug("compute-discovery", "SMBIOS parser initialized");
    }
    catch (const std::exception& e) {
        log_warning("compute-discovery", "Unable to read MDR region: " << e.what() << "; SMBIOS parser not initialized!");
    }

    try {
        auto iscsi_mdr_accessor = m_mdr_accessor_factory->create(get_platform_id(),
                                                                 m_management_controller,
                                                                 ipmi::command::sdv::DataRegionId::ISCSI_BOOT_OPTIONS);
        set_up_iscsi_data(iscsi_mdr_accessor->get_mdr_region());
        log_debug("compute-discovery", "iSCSI parser initialized");
    }
    catch (const std::exception& e) {
        log_warning("compute-discovery", "Unable to read MDR region: " << e.what() << "; iSCSI parser not initialized!");
    }

    try {
        auto acpi_mdr_accessor = m_mdr_accessor_factory->create(get_platform_id(),
                                                                m_management_controller,
                                                                ipmi::command::sdv::DataRegionId::ACPI_TABLE);
        set_up_acpi_data(acpi_mdr_accessor->get_mdr_region());
        log_debug("compute-discovery", "ACPI parser initialized");
    }
    catch (const std::exception& e) {
        log_warning("compute-discovery", "Unable to read MDR region: " << e.what() << "; ACPI parser not initialized!");
    }
}


bool GenericDiscoverer::discover_manager(Manager& manager) {
    generic::request::GetDeviceGuid get_guid_request{};
    generic::response::GetDeviceGuid get_guid_response{};

    generic::request::GetDeviceId get_id_request{};
    generic::response::GetDeviceId get_id_response{};

    try {
        log_debug("legacy-discovery", "Sending " << get_guid_request.get_command_name());
        get_management_controller().send(get_guid_request, get_guid_response);
        log_debug("legacy-discovery", "Sending " << get_id_request.get_command_name());
        get_management_controller().send(get_id_request, get_id_response);

        ManagerBuilder::update_device_id_data(manager, get_id_response);
        ManagerBuilder::update_guid_data(manager, get_guid_response);
    }
    catch (const ipmi::ResponseError& response_error) {
        log_ipmi_error(response_error);
        return false;
    }
    catch (const std::runtime_error& e) {
        log_discovery_error(e, manager.get_component().to_string());
        return false;
    }

    return true;
}


bool GenericDiscoverer::discover_chassis(agent_framework::model::Chassis& chassis) {
    bool smbios_successful{true};
    bool ipmi_successful{true};

    if (!get_smbios_parser()) {
        smbios_successful = false;
    }
    else {
        try {
            auto smbios_module = get_smbios_parser()->get_all<SMBIOS_MODULE_INFO_DATA>();

            if (smbios_module.size() > 1) {
                log_warning("smbios-discovery",
                            "Only one SMBIOS module information record expected. Number of records: "
                                << smbios_module.size());
            }
            else if (smbios_module.empty()) {
                throw std::runtime_error("There are no SMBIOS modules information in the SMBIOS table.");
            }

            const auto& module = smbios_module[0];

            ChassisBuilder::update_smbios_module_info(chassis, module);

            auto smbios_chassis_array = get_smbios_parser()->get_all<SMBIOS_SYSTEM_ENCLOSURE_OR_CHASSIS>(
                &module.data.chassis_handle);

            if (smbios_chassis_array.size() > 1) {
                log_warning("smbios-discovery",
                            "Only one system enclosure or chassis information record expected. Number of records: "
                                << smbios_chassis_array.size());
            }
            else if (smbios_chassis_array.empty()) {
                log_warning("smbios-discovery",
                            "There are no system enclosures or chassis in SMBIOS table.");
            }
            else {
                ChassisBuilder::update_smbios_system_enclosure_or_chassis_info(chassis, smbios_chassis_array[0]);
            }

            log_info("smbios-discovery", "Chassis SMBIOS discovery successful.");
        }
        catch (const std::exception& e) {
            log_error("smbios-discovery", "Chassis SMBIOS discovery error: " << e.what());
            smbios_successful = false;
        }
    }

    if (!smbios_successful) {
        generic::request::GetDeviceId get_device_id_request{};
        generic::response::GetDeviceId get_device_id_response{};

        try {
            log_debug("legacy-discovery", "Sending " << get_device_id_request.get_command_name());
            get_management_controller().send(get_device_id_request, get_device_id_response);

            ChassisBuilder::update_device_id(chassis, get_device_id_response);
        }
        catch (const ipmi::ResponseError& response_error) {
            log_ipmi_error(response_error);
            ipmi_successful = false;
        }
        catch (const std::runtime_error& e) {
            log_discovery_error(e, chassis.get_component().to_string());
            ipmi_successful = false;
        }
    }

    return smbios_successful || ipmi_successful;
}


bool GenericDiscoverer::discover_thermal_zone(agent_framework::model::ThermalZone& /* zone */) {
    return true;
}


bool GenericDiscoverer::discover_power_zone(agent_framework::model::PowerZone& /* zone */) {
    return true;
}


bool GenericDiscoverer::discover_system(agent_framework::model::System& system) {
    bool smbios_successful{true};
    bool ipmi_successful{true};

    if (!get_smbios_parser()) {
        smbios_successful = false;
    }
    else {
        try {
            auto smbios_systems = get_smbios_parser()->get_all<SMBIOS_SYSTEM_INFO_DATA>();
            auto smbios_modules = get_smbios_parser()->get_all<SMBIOS_MODULE_INFO_DATA>();
            auto smbios_bioses = get_smbios_parser()->get_all<SMBIOS_BIOS_INFO_DATA>();
            auto smbios_txts = get_smbios_parser()->get_all<SMBIOS_TXT_INFO_DATA>();
            auto smbios_processors = get_smbios_parser()->get_all<SMBIOS_PROCESSOR_INFO_DATA>();
            auto smbios_performance_configurations = get_smbios_parser()->get_all<SMBIOS_SPEED_SELECT_INFO_DATA>();

            if (smbios_systems.size() > 1) {
                log_warning("smbios-discovery",
                            "SMBIOS: Only one System information record expected. Number of records: "
                                << smbios_systems.size());
            }
            else if (smbios_systems.empty()) {
                throw std::runtime_error("SMBIOS: Unable to discover System information.");
            }
            else {
                SystemBuilder::update_smbios_system_info(system, smbios_systems[0]);
                log_info("smbios-discovery", "Discovered system with GUID: " << system.get_guid());
            }

            if (smbios_modules.size() > 1) {
                log_warning("smbios-discovery",
                            "Only one Module information record expected. Number of records: "
                                << smbios_modules.size());
            }
            else if (smbios_modules.empty()) {
                log_warning("smbios-discovery", "There are no Modules in SMBIOS table.");
            }
            else {
                SystemBuilder::update_smbios_module_info(system, smbios_modules[0]);
            }

            if (smbios_bioses.size() > 1) {
                log_warning("smbios-discovery",
                            "SMBIOS: Only one BIOS information record expected. Number of records: "
                                << smbios_bioses.size());
            }
            else if (smbios_bioses.empty()) {
                log_warning("smbios-discovery", "SMBIOS: Unable to discover system BIOS information.");
            }
            else {
                SystemBuilder::update_smbios_bios_info(system, smbios_bioses[0]);
            }

            if (1 != smbios_txts.size()) {
                log_warning("smbios-discovery",
                            "Not exactly 1 TXT structure (" << smbios_txts.size() << " found).");
            }
            else {
                auto txt = smbios_txts.front();
                SystemBuilder::update_smbios_txt_info(system, txt);
            }

            if (!smbios_performance_configurations.empty()) {
                if (smbios_performance_configurations.size() != smbios_processors.size()) {
                    log_warning("smbios-discovery", "The number of Speed Select configurations not equal to CPU count.");
                }
                if (smbios_performance_configurations[0].configs.size() > 1) {
                    log_info("smbios-discovery", "Detected Speed Select compatible system.");
                    SystemBuilder::update_smbios_performance_configurations(system, smbios_performance_configurations[0]);
                }
                else {
                    log_info("smbios-discovery", "Only 1 Speed Select configuration found - skipping.");
                }
            } else {
                log_info("smbios-discovery", "System is not Speed Select compatible.");
            }

            SystemBuilder::update_smbios_pcie_info(system, get_smbios_parser()->get_all<SMBIOS_PCIE_INFO_DATA>());

            log_info("smbios-discovery", "System discovery successful.");
        }
        catch (const std::exception& e) {
            log_error("smbios-discovery", "System SMBIOS discovery error: " << e.what());
            smbios_successful = false;
        }
    }

    if (!smbios_successful) {
        generic::request::GetSystemGuid get_system_guid_request{};
        generic::response::GetSystemGuid get_system_guid_response{};

        try {
            log_debug("legacy-discovery", "Sending " << get_system_guid_request.get_command_name());
            get_management_controller().send(get_system_guid_request, get_system_guid_response);

            SystemBuilder::update_system_guid(system, get_system_guid_response);
        }
        catch (const ipmi::ResponseError& response_error) {
            log_ipmi_error(response_error);
            ipmi_successful = false;
        }
        catch (const std::runtime_error& e) {
            log_discovery_error(e, system.get_component().to_string());
            ipmi_successful = false;
        }
    }

    return ipmi_successful || smbios_successful;
}


bool GenericDiscoverer::discover_pcie_devices(std::vector<PcieDevice>& devices, const Uuid& parent_uuid,
                                              const Uuid& chassis_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    auto smbios_devices = get_smbios_parser()->get_all<SMBIOS_PCIE_INFO_DATA>();
    for (const auto& device_data : smbios_devices) {
        auto pcie_device = PcieDeviceBuilder::build_default(parent_uuid);
        PcieDeviceBuilder::update_smbios_data(pcie_device, device_data);
        PcieDeviceBuilder::update_chassis_link(pcie_device, chassis_uuid);
        devices.emplace_back(std::move(pcie_device));
    }

    return true;
}


bool GenericDiscoverer::discover_pcie_functions(std::vector<PcieFunction>& functions,
                                                const std::vector<PcieDevice>& devices) {
    if (!get_smbios_parser()) {
        return false;
    }

    auto smbios_devices = get_smbios_parser()->get_all<SMBIOS_PCIE_INFO_DATA>();
    for (size_t i = 0; i < smbios_devices.size(); ++i) {
        auto pcie_function = PcieFunctionBuilder::build_default(devices[i].get_uuid());
        PcieFunctionBuilder::update_smbios_data(pcie_function, smbios_devices[i]);
        functions.emplace_back(std::move(pcie_function));
    }

    return true;
}


bool GenericDiscoverer::discover_processors(std::vector<agent_framework::model::Processor>& processors,
                                            const Uuid& parent_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    try {
        auto smbios_processors = get_smbios_parser()->get_all<SMBIOS_PROCESSOR_INFO_DATA>();
        auto smbios_fpgas = get_smbios_parser()->get_all<SMBIOS_FPGA_DATA>();
        auto smbios_fpgas_oem = get_smbios_parser()->get_all<SMBIOS_FPGA_DATA_OEM>();
        auto cpu_ids_v1 = get_smbios_parser()->get_all<SMBIOS_CPUID_DATA>();
        auto cpu_ids_v2 = get_smbios_parser()->get_all<SMBIOS_CPUID_DATA_V2>();

        for (const auto& cpu : smbios_processors) {
            auto processor = ProcessorBuilder::build_default(parent_uuid);
            ProcessorBuilder::update_smbios_processor_data(processor, cpu);
            ProcessorBuilder::update_smbios_cpu_id_data(processor, cpu_ids_v1, cpu_ids_v2);
            processors.emplace_back(std::move(processor));
        }

        for (const auto& smbios_fpga : smbios_fpgas) {
            auto fpga = ProcessorBuilder::build_default(parent_uuid);
            ProcessorBuilder::update_smbios_fpga_data(fpga, smbios_fpga);
            processors.emplace_back(fpga);
        }

        for (const auto& smbios_fpga_oem : smbios_fpgas_oem) {
            auto fpga = ProcessorBuilder::build_default(parent_uuid);
            ProcessorBuilder::update_smbios_fpga_data(fpga, smbios_fpga_oem);
            processors.emplace_back(fpga);
        }

        log_info("smbios-discovery", "Processor discovery successful.");
    }
    catch (const std::exception& e) {
        log_error("smbios-discovery", "Processor SMBIOS discovery error: " << e.what());
        return false;
    }

    return true;
}


bool GenericDiscoverer::discover_memory(std::vector<agent_framework::model::Memory>& memories,
                                        const Uuid& parent_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    try {
        auto arrays = get_smbios_parser()->get_all<SMBIOS_PHYSICAL_MEMORY_ARRAY>();
        auto devices = get_smbios_parser()->get_all<SMBIOS_MEMORY_DEVICE>();
        auto devices_extended = get_smbios_parser()->get_all<SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>();

        if (devices.empty()) {
            log_info("smbios-discovery", "No Memory Device found in SMBIOS");
            return false;
        }

        for (auto& device : devices) {
            auto memory = MemoryBuilder::build_default(parent_uuid);
            MemoryBuilder::update_smbios_data(memory, device, arrays);

            for (auto& device_extended : devices_extended) {
                if (memory.get_smbios_handle() == device_extended.data.memory_device_handle) {
                    MemoryBuilder::update_smbios_memory_device_extended_data(memory, device_extended);
                }
            }

            if (helpers::is_non_volatile_memory(device.data)) {
                log_debug("smbios-discovery", "Detected Non-Volatile Memory Device - Intel Optane DC Persistent Memory Module");
                MemoryBuilder::update_general_dcpmem_data(memory);

                auto acpi_parser = get_acpi_parser();
                if (acpi_parser) {
                    // ACPI Parser initialized with MDR data - check for supported tables
                    if (acpi_parser->prepare_if_structure_supported(acpi::structs::NFIT)) {
                        auto nfit_spa_range_structures =
                            acpi_parser->get_all<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>();
                        auto nfit_nvdimm_region_mapping_structures =
                            acpi_parser->get_all<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>();
                        auto nfit_nvdimm_control_region_structures =
                            acpi_parser->get_all<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>();
                        auto nfit_block_data_window_region_structures =
                            acpi_parser->get_all<acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE>();
                        auto nfit_platform_capabilities_structures =
                            acpi_parser->get_all<acpi::structs::NFIT_PLATFORM_CAPABILITIES_STRUCTURE>();

                        // Get Control Region for a given Memory
                        const auto& nvdimm_control_region = helpers::get_nvdimm_control_region(memory,
                                                                                               nfit_nvdimm_control_region_structures);

                        // Get all Region Mappings for given Control Region
                        const auto& nvdimm_region_mappings = helpers::get_nvdimm_region_mappings(nvdimm_control_region,
                                                                                                 nfit_nvdimm_region_mapping_structures);

                        MemoryBuilder::update_acpi_nfit_data(memory, nvdimm_control_region, nvdimm_region_mappings, nfit_spa_range_structures);
                    }

                    if (acpi_parser->prepare_if_structure_supported(acpi::structs::PCAT)) {
                        auto pcat_platform_capability_information_structures =
                            acpi_parser->get_all<acpi::structs::PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE>();
                        auto pcat_socket_sku_information_structures =
                            acpi_parser->get_all<acpi::structs::PCAT_SOCKET_SKU_INFORMATION_STRUCTURE>();
                    }
                }
            }

            memories.emplace_back(memory);
        }

        return true;
    }
    catch (const std::exception& e) {
        log_error("smbios-discovery", "Memory discovery error: " << e.what());
        return false;
    }
}


bool GenericDiscoverer::discover_memory_domains(std::vector<agent_framework::model::MemoryDomain>& memory_domains,
                                                const Uuid& parent_uuid) {
    try {
        auto memory_domain = MemoryDomainBuilder::build_default(parent_uuid);
        // TODO: Update MemoryDomain objects with relevant data
        memory_domains.emplace_back(std::move(memory_domain));

        return true;
    }
    catch (const std::exception& e) {
        log_error("acpi-discovery", "Memory Domains discovery error: " << e.what());
        return false;
    }
}


bool GenericDiscoverer::discover_memory_chunks(std::vector<agent_framework::model::MemoryChunks>& memory_chunks,
                                                const Uuid& parent_uuid) {
    try {
        auto memory_chunk = MemoryChunksBuilder::build_default(parent_uuid);
        // TODO: Update MemoryChunks objects with relevant data
        memory_chunks.emplace_back(std::move(memory_chunk));

        return true;
    }
    catch (const std::exception& e) {
        log_error("acpi-discovery", "Memory Chunks discovery error: " << e.what());
        return false;
    }
}


bool GenericDiscoverer::discover_storage_subsystem(agent_framework::model::StorageSubsystem& /*storage*/) {
    return true;
}


bool GenericDiscoverer::discover_drives(std::vector<agent_framework::model::Drive>& drives,
                                        const Uuid& parent_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    try {
        auto smbios_drives = get_smbios_parser()->get_all<SMBIOS_STORAGE_INFO_DATA>();
        auto smbios_drives_v2 = get_smbios_parser()->get_all<SMBIOS_STORAGE_INFO_DATA_V2>();
        auto smbios_storage_devices = get_smbios_parser()->get_all<SMBIOS_STORAGE_DEVICE_INFO_DATA>();

        for (const auto& smbios_drive : smbios_drives) {
            Drive drive = DriveBuilder::build_default(parent_uuid);

            DriveBuilder::update_smbios_data(drive, smbios_drive);

            drives.emplace_back(std::move(drive));
        }

        for (const auto& smbios_drive_v2 : smbios_drives_v2) {
            auto drive = DriveBuilder::build_default(parent_uuid);

            DriveBuilder::update_smbios_data(drive, smbios_drive_v2);

            drives.emplace_back(std::move(drive));
        }

        for (const auto& smbios_storage_device : smbios_storage_devices) {
            auto drive = DriveBuilder::build_default(parent_uuid);

            DriveBuilder::update_smbios_data(drive, smbios_storage_device);

            if (!drive.get_fru_info().get_serial_number().has_value() ||
                !drive.get_fru_info().get_serial_number()->empty()) {
                // skip drive with empty serial number (happens for drives attached using a PCIe Switch)
                drives.emplace_back(std::move(drive));
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        log_error("smbios-discovery", "Drives SMBIOS discovery error: " << e.what());
        return false;
    }
}


bool GenericDiscoverer::discover_network_interfaces(
    std::vector<agent_framework::model::NetworkInterface>& network_interfaces, const Uuid& parent_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    try {
        auto smbios_network_interfaces = get_smbios_parser()->get_all<SMBIOS_NIC_INFO_DATA>();
        auto smbios_network_interfaces_v2 = get_smbios_parser()->get_all<SMBIOS_NIC_INFO_DATA_V2>();

        for (const auto& smbios_network_interface : smbios_network_interfaces) {
            auto network_interface = NetworkInterfaceBuilder::build_default(parent_uuid);

            NetworkInterfaceBuilder::update_smbios_data(network_interface, smbios_network_interface);

            if (network_interface.get_mac_address().has_value()) {
                // skip interface with invalid MAC address (happens for rrc interface with BIOS in legacy mode)
                network_interfaces.emplace_back(std::move(network_interface));
            }
        }

        for (const auto& smbios_network_interface_v2 : smbios_network_interfaces_v2) {
            auto network_interface = NetworkInterfaceBuilder::build_default(parent_uuid);

            NetworkInterfaceBuilder::update_smbios_data(network_interface, smbios_network_interface_v2);

            if (network_interface.get_mac_address().has_value()) {
                // skip interface with invalid MAC address (happens for rrc interface with BIOS in legacy mode)
                network_interfaces.emplace_back(std::move(network_interface));
            }
        }

        if (1 == network_interfaces.size()) {
            // @TODO: take care of the corner case when only one interface is discovered
        }

        return true;
    }
    catch (const std::exception& e) {
        log_error("smbios-discovery", "Network interfaces SMBIOS discovery error: " << e.what());
        return false;
    }
}


bool GenericDiscoverer::discover_network_device(agent_framework::model::NetworkDevice& /*network_device*/) {
    return false;
}


bool GenericDiscoverer::discover_network_device_function(agent_framework::model::NetworkDeviceFunction& ndf) {
    if (!get_iscsi_parser()) {
        return false;
    }

    try {
        auto initiators = get_iscsi_parser()->get_all<iscsi::structs::ISCSI_MDR_INITIATOR>();
        auto targets = get_iscsi_parser()->get_all<iscsi::structs::ISCSI_MDR_TARGET>();
        auto attempts = get_iscsi_parser()->get_all<iscsi::structs::ISCSI_MDR_ATTEMPT>();
        auto versions = get_iscsi_parser()->get_all<iscsi::structs::ISCSI_MDR_VERSION>();
        ndf.set_device_enabled(false);

        // no iSCSI configuration available
        if (initiators.size() == 0 && targets.size() == 0 && attempts.size() == 0 && versions.size() == 1) {
            NetworkDeviceFunctionBuilder::clear_iscsi_mdr_data(ndf);
        }
        else {

            // currently only the same number of initiator, target and attempt structures in MDR are supported
            if (initiators.size() != targets.size() || targets.size() != attempts.size()) {
                log_error("iscsi-discovery",
                          "iSCSI MDR configuration supports an equal number of initiator, target and attempt structures,"
                              " configured initiators: " << initiators.size() <<
                                                         " configured targets: " << targets.size() <<
                                                         " configured attempts: " << attempts.size());
                return false;
            }

            // attempt handles must match initiator and target handles
            for (decltype(initiators)::size_type i = 0; i < initiators.size(); i++) {
                if ((attempts[i].data.initiator_handle != initiators[i].header.handle)
                    && (attempts[i].data.target_handle != targets[i].header.handle)) {
                    log_error("iscsi-discovery",
                              "Initiator, target and attempt handles do not match,"
                                  " attempt: (" << attempts[i].data.initiator_handle << ", "
                                                << attempts[i].data.target_handle << ") initiator, target: ("
                                                << initiators[i].header.handle << ", " << targets[i].header.handle
                                                << ")");
                    return false;
                }
            }

            NetworkDeviceFunctionBuilder::update_iscsi_mdr_data(ndf, initiators.front(), targets.front());
            // special case, where when writing, MAC address was null. Show the same state on REST.
            if (initiators.size() != 1) {
                log_debug("iscsi-discovery", "Multiple iSCSI MDR structures available, setting MAC address to null");
                ndf.set_mac_address({});
            }
            ndf.set_device_enabled(true);
        }

    }
    catch (const std::exception& e) {
        log_error("iscsi-discovery", "iSCSI discovery error: " << e.what());
        return false;
    }

    // enable the NetworkDeviceFunction
    auto status = ndf.get_status();
    status.set_state(enums::State::Enabled);
    ndf.set_status(status);

    return true;
}


bool GenericDiscoverer::discover_bios(agent_framework::model::System& /*system*/) {
    return false;
}


bool GenericDiscoverer::discover_cable_id(agent_framework::model::System& /*system*/) {
    return false;
}


bool GenericDiscoverer::discover_trusted_modules(std::vector<agent_framework::model::TrustedModule>& trusted_modules,
                                                 const Uuid& parent_uuid) {
    if (!get_smbios_parser()) {
        return false;
    }

    auto smbios_tpms = get_smbios_parser()->get_all<SMBIOS_TPM_INFO_DATA>();
    for (const auto& smbios_tpm : smbios_tpms) {
        auto trusted_module = TrustedModuleBuilder::build_default(parent_uuid);
        TrustedModuleBuilder::update_smbios_tpm_info(trusted_module, smbios_tpm);

        trusted_modules.push_back(std::move(trusted_module));
    }
    log_info("smbios-discovery", "TPM discovery was successful.");

    return true;
}


bool GenericDiscoverer::discover_user_mode(agent_framework::model::System& /*system*/) {
    return false;
}


bool GenericDiscoverer::set_rackscale_mode(agent_framework::model::System& /*system*/) {
    return false;
}


GenericDiscoverer::Ptr DiscovererFactory::create(ipmi::command::generic::BmcInterface bmc_interface,
                                                 ipmi::IpmiController& ipmi_controller,
                                                 ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory) const {
    switch (bmc_interface) {
        case ipmi::command::generic::BmcInterface::RSA_1_2:
            throw std::runtime_error("Unsupported platform");
        case ipmi::command::generic::BmcInterface::RSD_2_2:
            throw std::runtime_error("Unsupported platform");
        case ipmi::command::generic::BmcInterface::RSD_2_4:
            return std::make_shared<PurleyDiscoverer>(ipmi_controller, mdr_accessor_factory);
        case ipmi::command::generic::BmcInterface::UNKNOWN:
        default:
            break;
    }
    throw std::runtime_error("Unknown platform type");
}


DiscovererFactory::~DiscovererFactory() {}
