/*!
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
 * */

#include "generate_data.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include <iostream>

using namespace agent_framework::module;
using namespace agent_framework::model;

using ComputeComponents = agent_framework::module::ComputeManager;

namespace {

std::uint32_t MEMORY_SOCKET = 0;
std::uint32_t PROCESSOR_SOCKET = 0;
std::uint32_t MEMORY_CHUNK_UID = 0;

System make_system(const std::string& parent, const std::string& chassis) {
        System system{parent};
        system.set_uefi_target("/");
        system.set_bios_version("A20F21_A0");
        system.set_oem(agent_framework::model::attribute::Oem{});
        system.set_boot_override(enums::BootOverride::Once);
        system.set_boot_override_target(enums::BootOverrideTarget::Hdd);

        system.add_boot_override_supported(enums::BootOverrideSupported::Hdd);
        system.add_boot_override_supported(enums::BootOverrideSupported::Pxe);

        system.set_power_state(enums::PowerState::On);
        system.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        system.set_chassis(chassis);

        attribute::FruInfo fru_info;
        fru_info.set_serial_number("123fed3029c-b23394-12");
        fru_info.set_manufacturer("Intel Corporation");
        fru_info.set_model_number("E323");
        fru_info.set_part_number("29ee2220939");
        system.set_fru_info(std::move(fru_info));
        system.set_sku("SKU");
        system.set_asset_tag("free form asset tag");
        system.set_indicator_led(enums::IndicatorLed::Off);
        system.add_usb_device(attribute::UsbDevice("8087","0024"));
        system.add_usb_device(attribute::UsbDevice("1d6b","0002"));
        system.add_pci_device(attribute::PciDevice("8086","0023"));
        system.add_pci_device(attribute::PciDevice("1d5b","0003"));

        system.add_collection(attribute::Collection(
            enums::CollectionName::Processors,
            enums::CollectionType::Processors,
            "slotMask"
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::Memories,
            enums::CollectionType::Dimms,
            "slotMask"
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::MemoryChunks,
            enums::CollectionType::MemoryChunks,
            "slotMask"
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::StorageControllers,
            enums::CollectionType::StorageControllers,
            "slotMask"
        ));
        system.add_collection(attribute::Collection(
            enums::CollectionName::NetworkInterfaces,
            enums::CollectionType::NetworkInterfaces,
            "slotMask"
        ));
        return system;
}

Drive make_drive(const std::string& parent) {
        Drive drive{parent};
        drive.set_type(enums::DriveType::SSD);
        drive.set_oem(agent_framework::model::attribute::Oem{});
        drive.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        drive.set_interface(enums::DriveInterface::SATA);
        drive.set_capacity_gb(500);
        drive.set_physical_id("0.1.0");

        attribute::FruInfo fru_info;
        fru_info.set_serial_number("123fed3029c-b23394-12");
        fru_info.set_manufacturer("Intel Corporation");
        fru_info.set_model_number("E323");
        fru_info.set_part_number("29ee2220939");
        drive.set_fru_info(std::move(fru_info));

        return drive;
}

Manager make_manager() {
        Manager manager{};
        manager.set_manager_type(enums::ManagerInfoType::BMC);
        manager.set_manager_model("Simple IPMI compilant");
        // UID of chassis cannot be given here properly
        manager.set_location(OptionalField<string>());
        manager.set_firmware_version("2.58");
        manager.set_ipv4_address("1.1.2.1");
        manager.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        manager.set_oem(agent_framework::model::attribute::Oem{});
        manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::IPMI, 0, false));
        manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::SSH, 0, false));
        manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::Telnet, 0, false));

        attribute::GraphicalConsole graphical;
        graphical.set_enabled(true);
        graphical.set_max_sessions(10);
        attribute::Array<enums::GraphicalConsoleSupprtedType> gc;
        gc.add_entry(enums::GraphicalConsoleSupprtedType::KVMIP);
        graphical.set_types_supported(std::move(gc));
        manager.set_graphical_console(std::move(graphical));

        agent_framework::model::attribute::SerialConsole serial{};
        serial.set_bitrate(115200);
        serial.set_data_bits(8);
        serial.set_enabled(true);
        serial.set_flow_control(enums::SerialConsoleFlowControl::None);
        serial.set_parity(enums::SerialConsoleParity::None);
        serial.set_pin_out(enums::SerialConsolePinOut::Cisco);
        serial.set_stop_bits(1);
        serial.set_signal_type(enums::SerialConsoleSignalType::Rs232);
        serial.set_max_sessions(1);
        attribute::Array<enums::SerialConsoleSupprtedType> sc;
        sc.add_entry(enums::SerialConsoleSupprtedType::IPMI);
        sc.add_entry(enums::SerialConsoleSupprtedType::SSH);
        serial.set_types_supported(std::move(sc));
        manager.set_serial_console(std::move(serial));

        attribute::CommandShell cmd_shell;
        cmd_shell.set_enabled(true);
        cmd_shell.set_max_sessions(20);
        attribute::Array<enums::CommandShellSupprtedType> cc;
        cc.add_entry(enums::CommandShellSupprtedType::Telnet);
        cmd_shell.set_types_supported(std::move(cc));
        manager.set_command_shell(std::move(cmd_shell));

        manager.add_collection(attribute::Collection(
            enums::CollectionName::Systems,
            enums::CollectionType::Systems,
            "slotMask"
        ));
        manager.add_collection(attribute::Collection(
            enums::CollectionName::Chassis,
            enums::CollectionType::Chassis,
            "SlotMask"));
        return manager;
}

Dimm make_dimm(const std::string& parent) {
        Dimm dimm{parent};

        dimm.set_memory_type(enums::MemoryType::DRAM);
        dimm.set_dimm_type(enums::DimmType::DDR4_SDRAM);
        dimm.set_module_type(enums::DimmModuleType::SO_UDIMM_72b);
        dimm.set_dimm_media(std::move(std::vector<enums::DimmMedia> {
            enums::DimmMedia::DRAM
        }));
        dimm.set_capacity_mb(16 * 1024);
        dimm.set_data_width_bits(8);
        dimm.set_bus_width_bits(8);

        attribute::FruInfo fru_info;
        fru_info.set_serial_number("123fed3029c-b23394-12");
        fru_info.set_manufacturer("Intel Corporation");
        fru_info.set_model_number("E323");
        fru_info.set_part_number("29ee2220939");
        dimm.set_fru_info(std::move(fru_info));

        dimm.set_allowed_speeds_mhz(std::vector<std::uint32_t> {
            2133,
            2400,
            2667
        });
        dimm.set_firmware_revision("Rev 0.1");
        dimm.set_firmware_api_version("Api 1.2");
        dimm.set_memory_classes(std::vector<enums::MemoryClass> {
            enums::MemoryClass::Volatile,
            enums::MemoryClass::Block
        });
        dimm.set_vendor_id("vendorX");
        dimm.set_device_id("deviceX");
        dimm.set_rank_count(1);
        dimm.set_device_locator("PROC 1 DIMM 1");

        attribute::DimmLocation location;
        location.set_socket(++MEMORY_SOCKET);
        location.set_controller(1);
        location.set_channel(1);
        location.set_slot(1);
        dimm.set_dimm_location(std::move(location));

        dimm.set_error_correction(enums::ErrorCorrection::MultiBitECC);
        dimm.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        dimm.set_operating_speed_mhz(2400);

        attribute::Region region1, region2;
        region1.set_region_id("1");
        region1.set_memory_type(enums::MemoryClass::Volatile);
        region1.set_offset_mb(0);
        region1.set_size_mb(8000);
        region2.set_region_id("2");
        region2.set_memory_type(enums::MemoryClass::Block);
        region2.set_offset_mb(8000);
        region2.set_size_mb(8000);
        dimm.add_region(std::move(region1));
        dimm.add_region(std::move(region2));

        dimm.set_memory_modes(std::move(std::vector<enums::MemoryMode> {
            enums::MemoryMode::Volatile,
            enums::MemoryMode::PMEM
        }));
        dimm.set_oem(agent_framework::model::attribute::Oem{});
        dimm.set_voltage_volt(1.35);
        dimm.set_min_voltage_volt(1.15);
        dimm.set_max_voltage_volt(1.45);

        return dimm;
}

MemoryChunk make_memory_chunk(const std::string& parent, const Dimm& dimm) {
        MemoryChunk chunk{parent};

        chunk.set_chunk_name("Volatile");
        chunk.set_chunk_id(++MEMORY_CHUNK_UID);
        chunk.set_type(enums::MemoryChunkType::Volatile);
        chunk.set_mirrored(false);
        chunk.set_spare(false);
        std::uint32_t total_size{};
        for (const auto& region : dimm.get_regions()) {
            total_size += region.get_size_mb();
            chunk.add_interleave_set(std::move(
                agent_framework::model::attribute::InterleaveSet{
                    dimm.get_uuid(),
                    region.get_region_id()
                }));
        }
        chunk.set_chunk_size_mb(total_size);

        return chunk;
}

NetworkInterface make_network(const std::string& parent) {
        NetworkInterface interface{parent};

        interface.set_frame_size(1520);
        interface.set_oem(agent_framework::model::attribute::Oem{});
        interface.set_speed_mbps(1);
        interface.set_autosense(true);
        interface.set_mac_address("AA:BB:CC:DD:EE:FF");
        interface.set_factory_mac_address("AA:BB:CC:DD:EE:FF");
        interface.set_ipv6_default_gateway("fe80::1ec1:deff:febd:67e3");
        interface.set_max_ipv6_static_addresses(1);
        interface.set_default_vlan(2);
        interface.add_ipv4_address(attribute::Ipv4Address(
            "10.0.2.2",
            "255.255.255.0",
            enums::Ipv4AddressOrigin::DHCP,
            "10.0.2.1"
        ));
        interface.add_ipv6_address(attribute::Ipv6Address(
            "fe80::1ec1:deff:fe6f:1c37",
            16,
            enums::Ipv6AddressOrigin::DHCP,
            enums::Ipv6AddressState::Preferred
        ));
        interface.set_vlan_enable(true);
        interface.set_full_duplex(true);
        interface.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        interface.add_collection(attribute::Collection(
            enums::CollectionName::PortVlans,
            enums::CollectionType::PortVlans,
            "slotMask"
        ));

        return interface;

}

Processor make_processor(const std::string& parent) {
        Processor processor{parent};

        processor.set_socket(std::to_string(++PROCESSOR_SOCKET));
        processor.set_processor_type(enums::ProcessorType::CPU);
        processor.set_oem(agent_framework::model::attribute::Oem{});
        processor.set_processor_architecture(
            enums::ProcessorArchitecture::x86);
        processor.set_instruction_set(enums::ProcessorInstructionSet::x86_64);

        processor.add_capability("x86:EM64T");
        processor.add_capability("x86:MMX");

        processor.set_manufacturer(literals::Intel::INTEL_CORP);
        processor.set_model_name("Multi-Core Intel(R) Xeon(R) processor 7xxx Series");
        processor.set_model(enums::ProcessorModel::Unknown);

        attribute::CpuId cpu_id;
        cpu_id.set_vendor_id("GenuineIntel");
        cpu_id.set_numeric_id("329442");
        cpu_id.set_family("6");
        cpu_id.set_model("5");
        cpu_id.set_step("2");
        cpu_id.set_microcode_info("11");
        processor.set_cpu_id(std::move(cpu_id));

        processor.set_max_speed_mhz(3700);
        processor.set_total_cores(8);
        processor.set_enabled_cores(4);
        processor.set_total_threads(4);
        processor.set_enabled_threads(2);
        processor.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });
        return processor;
}

StorageController make_controller(const std::string& parent) {
        StorageController controller{parent};

        controller.set_interface(enums::DriveInterface::SAS);
        controller.set_oem(agent_framework::model::attribute::Oem{});
        controller.set_physical_id("pci@0000:01:00.0");
        controller.set_status({
            agent_framework::model::enums::State::Enabled,
            agent_framework::model::enums::Health::OK
        });

        attribute::FruInfo fru_info;
        fru_info.set_serial_number("123fed3029c-b23394-12");
        fru_info.set_manufacturer("Intel Corporation");
        fru_info.set_model_number("E323");
        fru_info.set_part_number("29ee2220939");
        controller.set_fru_info(std::move(fru_info));

        controller.add_collection(attribute::Collection(
            enums::CollectionName::Drives,
            enums::CollectionType::Drives,
            "slotMask"
        ));
        return controller;

}

Chassis make_chassis(const std::string& parent) {
    Chassis chassis{parent};
    chassis.set_status(attribute::Status(
        agent_framework::model::enums::State::Enabled,
        agent_framework::model::enums::Health::OK));
    chassis.set_type(enums::ChassisType::Blade);
    chassis.set_size(3);
    chassis.set_location_offset(1);
    chassis.set_parent_id(std::to_string(1));
    chassis.set_power_zone("");
    chassis.set_thermal_zone("");
    chassis.set_fru_info(attribute::FruInfo(
        "123fed3029c-b23094-12",
        "Intel Corporation",
        "E234",
        "29ee2220939"));
    chassis.set_sku("SKU");
    chassis.set_indicator_led(
        agent_framework::model::enums::IndicatorLed::Lit);
    chassis.set_asset_tag("Asset Tag");
    chassis.set_oem(attribute::Oem());
    return chassis;
}

void create_dimms(const std::string& parent) {
    for (auto i = 0u; i < 4u; ++i) {
        auto dimm = ::make_dimm(parent);
        ComputeComponents::get_instance()->
            get_dimm_manager().add_entry(dimm);
        log_debug(GET_LOGGER("rpc"), "Adding dimm: " << dimm.get_uuid());
        auto chunk = ::make_memory_chunk(parent, dimm);
        ComputeComponents::get_instance()->
            get_memory_chunk_manager().add_entry(chunk);
        log_debug(GET_LOGGER("rpc"), "Adding memory chunk: " << chunk.get_uuid());
    }
}

void create_processors(const std::string& parent) {
    for (auto i = 0u; i < 4u; ++i) {
        auto processor = ::make_processor(parent);
        ComputeComponents::get_instance()->
            get_processor_manager().add_entry(processor);
        log_debug(GET_LOGGER("rpc"), "Adding processor: " << processor.get_uuid());
    }
}

void create_networks(const std::string& parent) {
    for (auto i = 0u; i < 1u; ++i) {
        auto interface = ::make_network(parent);
        ComputeComponents::get_instance()->
            get_network_interface_manager().add_entry(interface);
        log_debug(GET_LOGGER("rpc"), "Adding network interface: " << interface.get_uuid());
    }
}

void create_drives(const std::string& parent) {
    for (auto i = 0u; i < 2u; ++i) {
        auto drive = ::make_drive(parent);
        ComputeComponents::get_instance()->
            get_drive_manager().add_entry(drive);
        log_debug(GET_LOGGER("rpc"), "Adding drive: " << drive.get_uuid());
    }
}

void create_controllers(const std::string& parent) {
    auto controller = ::make_controller(parent);
    ComputeComponents::get_instance()->
        get_storage_controller_manager().add_entry(controller);
    log_debug(GET_LOGGER("rpc"), "Adding controller: " << controller.get_uuid());
    create_drives(controller.get_uuid());
}

void create_systems(const std::string& parent, const std::string& chassis) {
    for (auto i = 0u; i < 4u; ++i) {
        auto system = ::make_system(parent, chassis);
        log_debug(GET_LOGGER("rpc"), "Adding system: " << system.get_uuid());
        ComputeComponents::get_instance()->
                get_system_manager().add_entry(system);
        create_dimms(system.get_uuid());
        create_processors(system.get_uuid());
        create_controllers(system.get_uuid());
        create_networks(system.get_uuid());
    }
}

std::string create_chassis(const std::string& parent) {
    auto chassis = ::make_chassis(parent);
    ComputeManager::get_instance()->
        get_chassis_manager().add_entry(chassis);
        log_debug(GET_LOGGER("rpc"), "Adding chassis: " << chassis.get_uuid());
    return chassis.get_uuid();
}

void create_manager() {
    for (auto i = 0u; i < 2u; ++i) {
        auto manager = ::make_manager();
        log_debug(GET_LOGGER("rpc"), "Adding mananger: " << manager.get_uuid());
        ComputeComponents::get_instance()->
                get_module_manager().add_entry(manager);
        auto chassis = create_chassis(manager.get_uuid());
        create_systems(manager.get_uuid(), chassis);
    }
}

}

void agent::compute::generate_data() {
    create_manager();
}
