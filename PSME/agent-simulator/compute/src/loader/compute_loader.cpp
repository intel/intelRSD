/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */


#include "loader/compute_loader.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/chassis.hpp"
#include "asset_configuration/asset_configuration.hpp"

using namespace agent::compute::loader;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::attribute;

using agent::AssetConfiguration;

ComputeLoader::~ComputeLoader() {}

class LoadModules {
public:
    using ComputeComponents = agent_framework::module::ComputeComponents;

    /* @TODO replace all literals with constants*/

    void read_asset(const xmlpp::Node* element) {
    if (element) {
            log_debug(GET_LOGGER("discovery"), "Reading asset.");
            auto manager = make_manager(element);
            read_chassis(manager,element);
            read_managers(manager, element);
            CommonComponents::get_instance()->
                    get_module_manager().add_entry(manager);
        }
    }

    void read_chassis(const Manager& manager, const xmlpp::Node* parent) {
        if (parent) {
            for (const auto& element : parent->get_children(literals::System::CHASSIS)) {
                auto chassis = make_chassis(manager, element);
                read_hard_drives(chassis, element);
                CommonComponents::get_instance()->
                        get_chassis_manager().add_entry(chassis);
                }
        }
    }

    Chassis make_chassis(const Manager& manager, const xmlpp::Node* element) {
        Chassis chassis{manager.get_uuid()};
        if (element) {
            std::string type = AssetConfiguration::read_string(element, literals::Chassis::TYPE);
            if (enums::ChassisType::is_allowable_value(type)) {
                chassis.set_type(enums::ChassisType::from_string(type));
            }
            log_debug(GET_LOGGER("discovery"), "Creating chassis.");
            chassis.set_status(make_status(element));
            chassis.set_location_offset(agent::AssetConfiguration::read_int(element, literals::Chassis::LOCATION_OFFSET));
            chassis.set_parent_id(agent::AssetConfiguration::read_string(element, literals::Chassis::PARENT_ID));
            chassis.add_collection(attribute::Collection(
                enums::CollectionName::Drives,
                enums::CollectionType::Drives,
                ""
            ));
            return chassis;
        }
        return chassis;
    }

    void read_managers(const Manager& parent_manager, const xmlpp::Node* parent) {
        if (parent) {
            log_debug(GET_LOGGER("discovery"), "Reading managers.");
            for (const auto& element : parent->get_children(literals::Manager::MANAGER)) {
                log_debug(GET_LOGGER("discovery"), "Reading manager.");
                auto manager = make_manager(parent_manager, element);
                read_chassis(manager,element);
                read_systems(manager, element);
                CommonComponents::get_instance()->
                        get_module_manager().add_entry(manager);
            }
        } else {
            log_warning(GET_LOGGER("discovery"), "No managers found.");
        }
    }

    Manager make_manager(const xmlpp::Node* element) {
        Manager manager{};
        manager.set_status(make_status(element));
        if (element) {
            manager.set_firmware_version(AssetConfiguration::
                    read_opt_string(element, literals::Manager::FIRMWARE_VERSION));
            manager.set_guid(AssetConfiguration::
                    read_opt_string(element, literals::Manager::GUID));
            manager.set_ipv4_address(AssetConfiguration::
                    read_opt_string(element, literals::Manager::IPv4_ADDRESS));
            std::string type = AssetConfiguration::
                read_string(element, literals::Manager::TYPE);
            if (enums::ManagerInfoType::is_allowable_value(type)) {
                manager.set_manager_type(enums::ManagerInfoType::
                    from_string(type));
            }
            manager.set_manager_model(AssetConfiguration::
                    read_opt_string(element, literals::Manager::MODEL));
            manager.set_serial_console(make_serial_console(element));

            log_debug(GET_LOGGER("discovery"), "Creating root manager.");
        }
        return manager;
    }

    Manager make_manager(const Manager& parent_manager,const xmlpp::Node* element) {
        Manager manager{parent_manager.get_uuid()};
        manager.set_status(make_status(element));
        if (element) {
            manager.set_firmware_version(AssetConfiguration::
                    read_opt_string(element, literals::Manager::FIRMWARE_VERSION));
            manager.set_guid(AssetConfiguration::
                    read_opt_string(element, literals::Manager::GUID));
            manager.set_ipv4_address(AssetConfiguration::
                    read_opt_string(element, literals::Manager::IPv4_ADDRESS));
            std::string type = AssetConfiguration::
                read_string(element, literals::Manager::TYPE);
            if (enums::ManagerInfoType::is_allowable_value(type)) {
                manager.set_manager_type(enums::ManagerInfoType::
                    from_string(type));
            }
            manager.set_manager_model(AssetConfiguration::
                    read_opt_string(element, literals::Manager::MODEL));
            manager.set_serial_console(make_serial_console(element));
            log_debug(GET_LOGGER("discovery"), "Creating manager.");
        }
        manager.add_collection(attribute::Collection(
            enums::CollectionName::Chassis,
            enums::CollectionType::Chassis,
            ""
        ));
        manager.add_collection(attribute::Collection(
            enums::CollectionName::Systems,
            enums::CollectionType::Systems,
            ""
        ));
        return manager;
    }

    void read_systems(Manager& manager, const xmlpp::Node* parent) {
        if (parent) {
            log_debug(GET_LOGGER("discovery"), "Reading compute systems.");
            for (const auto& element : parent->get_children("ComputeSystem")) {
                log_debug(GET_LOGGER("discovery"), "Reading compute system.");
                auto system = make_system(manager, element);
                read_network_interfaces(system, element);
                read_storage_subsystem(system, element);
                read_memory_modules(system, element);
                read_processors(system, element);
                read_usb_devices(system, element);
                read_pci_devices(system, element);

                CommonComponents::get_instance()->
                        get_system_manager().add_entry(system);
            }
        } else {
            log_warning(GET_LOGGER("discovery"), "No compute systems found.");
        }
    }

    System make_system(const Manager& manager, const xmlpp::Node* element) {
        System system{manager.get_uuid()};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating compute system.");
            system.set_fru_info(make_fru_info(element));
            system.set_bios_version(AssetConfiguration::
                    read_opt_string(element, literals::System::BIOS_VERSION));
            system.set_status(make_status(element));
            system.set_asset_tag(AssetConfiguration::
                    read_opt_string(element, literals::System::ASSET_TAG));
            system.set_guid(AssetConfiguration::
                    read_string(element, literals::System::GUID));
            if (!CommonComponents::get_instance()->get_chassis_manager().
                    get_keys(manager.get_uuid()).empty()) {
                auto chassis_uuid = CommonComponents::get_instance()->
                        get_chassis_manager().get_keys(manager.get_uuid()).front();
                system.set_chassis(chassis_uuid);
                auto chassis = CommonComponents::get_instance()->
                        get_chassis_manager().get_entry_reference(chassis_uuid);
                chassis->set_fru_info(system.get_fru_info());
            }
            system.add_collection(attribute::Collection(
                enums::CollectionName::StorageSubsystems,
                enums::CollectionType::StorageSubsystems,
                ""
            ));
            system.add_collection(attribute::Collection(
                enums::CollectionName::NetworkInterfaces,
                enums::CollectionType::NetworkInterfaces,
                ""
            ));
            system.add_collection(attribute::Collection(
                enums::CollectionName::Memory,
                enums::CollectionType::Memory,
                ""
            ));
            system.add_collection(attribute::Collection(
                enums::CollectionName::Processors,
                enums::CollectionType::Processors,
                ""
            ));
        }
        return system;
    }

    attribute::SerialConsole make_serial_console(const xmlpp::Node* element) {
        attribute::SerialConsole serial_console{};
        auto console = element->get_first_child("serialConsole");
        if (element) {
            serial_console.set_bitrate(AssetConfiguration::
                    read_opt_int(console,literals::SerialConsole::BITRATE));
            serial_console.set_data_bits(AssetConfiguration::
                    read_opt_int(console, literals::SerialConsole::DATA_BITS));
            serial_console.set_stop_bits(AssetConfiguration::
                    read_opt_int(console, literals::SerialConsole::STOP_BITS));
            serial_console.set_max_sessions(AssetConfiguration::
                    read_opt_int(console, literals::SerialConsole::MAX_SESSIONS));
            serial_console.set_enabled(AssetConfiguration::
                    read_opt_bool(console, literals::SerialConsole::ENABLED));

            log_debug(GET_LOGGER("discovery"), "Creating serial console.");
        }
        return serial_console;
    }

    void read_network_interfaces(const System& system, const xmlpp::Node* parent) {
        if (parent) {
            if (!parent->get_children("networkInterface").empty()) {
                log_debug(GET_LOGGER("discovery"), "Reading network interfaces.");
                for (const auto& element : parent->get_children("networkInterface")) {
                    log_debug(GET_LOGGER("discovery"), "Reading network interface.");
                     auto interface = make_network_interface(system, element);
                         get_manager<NetworkInterface>().add_entry(interface);
                }
            }
            else {
                log_warning(GET_LOGGER("discovery"), "No network interfaces found.");
            }
        }
        else {
            log_warning(GET_LOGGER("discovery"), "No network interfaces found.");
        }
    }

    NetworkInterface
    make_network_interface(const System & system, const xmlpp::Node* element) {
        NetworkInterface interface{system.get_uuid()};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating network interface.");
            interface.set_frame_size(AssetConfiguration::
                    read_int(element, literals::NetworkInterface::FRAME_SIZE));
            interface.set_speed_mbps(AssetConfiguration::
                    read_opt_int(element, literals::NetworkInterface::SPEED));
            interface.set_full_duplex(AssetConfiguration::
                    read_opt_bool(element, literals::NetworkInterface::FULL_DUPLEX));
            interface.set_autosense(AssetConfiguration::
                    read_opt_bool(element, literals::NetworkInterface::AUTO_SENSE));
            interface.set_mac_address(AssetConfiguration::
                    read_string(element, literals::NetworkInterface::MAC_ADDRESS));
            interface.set_factory_mac_address(AssetConfiguration::
                    read_string(element, literals::NetworkInterface::FACTORY_MAC_ADDRESS));
            interface.set_vlan_enable(AssetConfiguration::
                    read_opt_bool(element, literals::NetworkInterface::VLAN_ENABLE));
            read_ipv4_addresses(interface, element);
            read_ipv6_addresses(interface, element);
            interface.set_status(make_status(element));
        }
        return interface;
    }

    void read_ipv4_addresses(NetworkInterface& interface, const xmlpp::Node* parent) {
        if (parent) {
            for (const auto& element : parent->get_children(literals::
                                    NetworkInterface::IPv4_ADDRESS)) {
                interface.add_ipv4_address(make_ipv4_address(element));
            }
        }
    }

    void read_ipv6_addresses(NetworkInterface& interface, const xmlpp::Node* parent) {
        if (parent) {
            for (const auto& element : parent->get_children(literals::
                                    NetworkInterface::IPv6_ADDRESS)) {
                interface.add_ipv6_address(make_ipv6_address(element));
            }
        }
    }

    Ipv4Address make_ipv4_address(const xmlpp::Node* element) {
        Ipv4Address address{};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating ipv4 adress.");
            address.set_address(AssetConfiguration::
                    read_opt_string(element, literals::Ipv4Address::ADDRESS));
            address.set_subnet_mask(AssetConfiguration::
                    read_opt_string(element, literals::Ipv4Address::SUBNET_MASK));
            std::string origin = AssetConfiguration::
                    read_string(element, literals::Ipv4Address::ADDRESS_ORIGIN);
            if (enums::Ipv4AddressOrigin::is_allowable_value(origin)) {
                address.set_address_origin(enums::Ipv4AddressOrigin::
                        from_string(origin));
            }
            address.set_gateway(AssetConfiguration::
                    read_opt_string(element ,literals::Ipv4Address::GATEWAY));
        }
        return address;
    }

    Ipv6Address make_ipv6_address(const xmlpp::Node* element) {
        Ipv6Address address{};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating ipv6 adress.");
            address.set_address(AssetConfiguration::
                    read_opt_string(element, literals::Ipv6Address::ADDRESS));
            address.set_prefix_length(AssetConfiguration::
                    read_opt_int(element, literals::Ipv6Address::PREFIX_LENGTH));
            std::string origin = AssetConfiguration::
                    read_string(element, literals::Ipv6Address::ADDRESS_ORIGIN);
            if (enums::Ipv6AddressOrigin::is_allowable_value(origin)) {
                address.set_address_origin(enums::Ipv6AddressOrigin::
                        from_string(origin));
            }
            std::string state= AssetConfiguration::
                    read_string(element, literals::Ipv6Address::ADDRESS_STATE);
            if (enums::Ipv6AddressState::is_allowable_value(state)) {
                address.set_address_state(enums::Ipv6AddressState::
                        from_string(state));
            }
        }
        return address;
    }

    NeighborInfo make_neighbor_info(const xmlpp::Node* element) {
        NeighborInfo neighbor_info{};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating neighbor info.");
            auto info = element->get_first_child("neighborInfo");
            neighbor_info.set_switch_identifier(AssetConfiguration::
                    read_string(info,"switchIdentifier"));
            neighbor_info.set_port_identifier(AssetConfiguration::
                    read_string(element, "portIdentifier"));
        }
        return neighbor_info;
    }

    Status make_status(const xmlpp::Node* element) {
        if (element) {
            auto elem = element->get_first_child("status");
            if (elem) {
                return agent_framework::model::attribute::Status(
                        AssetConfiguration::read_string(elem, literals::Status::STATE),
                        AssetConfiguration::read_string(elem, literals::Status::HEALTH));
            }
        }
        return agent_framework::model::attribute::Status("Enabled", "OK");
    }

    void read_storage_subsystem(const System& system, const xmlpp::Node* element) {
        if (element) {
            for (const auto& elem : element->get_children("storageSubsystem")) {
                auto storage = make_storage_subsystem(system, elem);
                read_storage_controllers(storage, elem);
                CommonComponents::get_instance()->
                    get_storage_subsystem_manager().add_entry(storage);
            }
        }
    }

    void read_storage_controllers(const StorageSubsystem& storage, const xmlpp::Node* element) {
        if (element) {
            for (const auto& elem : element->get_children("storageController")) {
                auto controller = make_storage_controller(storage, elem);
                ComputeComponents::get_instance()->
                    get_storage_controller_manager().add_entry(controller);
            }
        }
    }

    StorageSubsystem make_storage_subsystem(const System& system, const xmlpp::Node* element) {
        StorageSubsystem storage{system.get_uuid()};
        storage.set_status(make_status(element));
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating storage subsystem.");
        }
        storage.add_collection(attribute::Collection(
            enums::CollectionName::StorageControllers,
            enums::CollectionType::StorageControllers,
            ""
        ));
        storage.add_collection(attribute::Collection(
            enums::CollectionName::Drives,
            enums::CollectionType::Drives,
            ""
        ));
        return storage;
    }

    StorageController
    make_storage_controller(const StorageSubsystem& storage, const xmlpp::Node* element) {
        StorageController controller{storage.get_uuid()};
        controller.set_status(make_status(element));
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating storage controller.");
            std::string interface = AssetConfiguration::
                    read_string(element, literals::StorageController::INTERFACE);
            if (enums::StorageProtocol::is_allowable_value(interface)) {
                controller.add_supported_device_protocol(enums::StorageProtocol::
                        from_string(interface));
            }
            controller.set_fru_info(make_fru_info(element));
            controller.set_physical_id(AssetConfiguration::
                    read_opt_string(element, "busInfo"));
        }
        return controller;
    }

    FruInfo make_fru_info(const xmlpp::Node* element) {
        FruInfo fru_info{};
        if (element) {
            auto elem = element->get_first_child(literals::FruInfo::FRU_INFO);
            if (elem) {
                fru_info.set_serial_number(AssetConfiguration::
                        read_opt_string(elem, literals::FruInfo::SERIAL));
                fru_info.set_manufacturer(AssetConfiguration::
                        read_opt_string(elem, literals::FruInfo::MANUFACTURER));
                fru_info.set_model_number(AssetConfiguration::
                        read_opt_string(elem, literals::FruInfo::MODEL));
                fru_info.set_part_number(AssetConfiguration::
                       read_opt_string(elem, literals::FruInfo::PART));
            }
        }
        return fru_info;
    }

    void read_hard_drives(const Chassis& chassis, const xmlpp::Node* element) {
        if (element) {
            for (const auto& elem : element->get_children(literals::Drive::DRIVE)) {
                CommonComponents::get_instance()->
                     get_drive_manager().add_entry(make_hard_drive(chassis, elem));
            }
        }
    }

    Drive make_hard_drive(const Chassis& chassis, const xmlpp::Node* element) {
        Drive hard_drive{chassis.get_uuid()};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating hard drive.");
            std::string interface = AssetConfiguration::
                    read_string(element, literals::Drive::INTERFACE);
            if (enums::StorageProtocol::is_allowable_value(interface)) {
                hard_drive.set_interface(enums::StorageProtocol::
                        from_string(interface));
            }
            std::string type = AssetConfiguration::
                    read_string(element, literals::Drive::TYPE);
            if (enums::DriveType::is_allowable_value(type)) {
                hard_drive.set_type(enums::DriveType::from_string(type));
            }
            hard_drive.set_capacity_gb(AssetConfiguration::
                    read_opt_double(element, literals::Drive::CAPACITY));
            hard_drive.set_rpm(AssetConfiguration::
                    read_opt_int(element, literals::Drive::RPM));
            hard_drive.set_firmware_version(AssetConfiguration::
                    read_opt_string(element, literals::Drive::FIRMWARE_VERSION));
            hard_drive.set_fru_info(make_fru_info(element));
            hard_drive.set_status(make_status(element));
        }
        return hard_drive;
    }

    void read_memory_modules(const System& system, const xmlpp::Node* element) {
        if (element) {
            for (const auto& elem : element->get_children(literals::Memory::MEMORY)) {
                 ComputeComponents::get_instance()->
                     get_memory_manager().add_entry(
                                make_memory(system, elem));
            }
        }
    }

    Memory make_memory(const System& system, const xmlpp::Node* element) {
        Memory memory{system.get_uuid()};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating memory module.");

            memory.set_device_locator(AssetConfiguration::
                    read_opt_string(element, literals::Memory::DEVICE_LOCATOR));
            memory.set_bus_width_bits(AssetConfiguration::
                    read_opt_int(element, literals::Memory::BUS_WIDTH_BITS));
            memory.set_data_width_bits(AssetConfiguration::
                    read_opt_int(element, literals::Memory::DATA_WIDTH_BITS));
            memory.set_capacity_mb(AssetConfiguration::
                    read_double(element, literals::Memory::CAPACITY_MB));
            memory.set_fru_info(make_fru_info(element));
            memory.set_operating_speed_mhz(AssetConfiguration::
                    read_opt_int(element, literals::Memory::OPERATING_SPEED_MHZ));
            memory.set_voltage_volt(AssetConfiguration::
                    read_opt_double(element,literals::Memory::VOLTAGE));
            memory.set_min_voltage_volt(AssetConfiguration::
                    read_opt_double(element, literals::Memory::MIN_VOLTAGE));
            memory.set_max_voltage_volt(AssetConfiguration::
                    read_opt_double(element, literals::Memory::MAX_VOLTAGE));
            memory.set_device_id(AssetConfiguration::
                    read_opt_string(element, literals::Memory::DEVICE_ID));
            memory.set_firmware_revision(AssetConfiguration::
                    read_opt_string(element, literals::Memory::FIRMWARE_REVISION));
            memory.set_status(make_status(element));

            attribute::Region region1;
            region1.set_region_id("1");
            region1.set_memory_type(enums::MemoryClass::Volatile);
            region1.set_offset_mb(0);
            if (memory.get_capacity_mb().has_value()) {
                region1.set_size_mb(memory.get_capacity_mb());
            }
            memory.add_region(std::move(region1));
        }
        return memory;
    }

    void read_processors(const System& system, const xmlpp::Node* element) {
        if (element) {
            for (const auto& elem : element->get_children(literals::Processor::PROCESSOR)) {
                 get_manager<Processor>().add_entry(make_processor(system, elem)); }
        }
    }

    Processor make_processor(const System& system, const xmlpp::Node* element) {
        Processor processor{system.get_uuid()};
        if (element) {
            log_debug(GET_LOGGER("discovery"), "Creating processor.");
            processor.set_manufacturer(AssetConfiguration::
                    read_opt_string(element, literals::Processor::MANUFACTURER));
            processor.set_enabled_cores(AssetConfiguration::
                    read_opt_int(element, literals::Processor::ENABLED_CORES));
            processor.set_total_cores(AssetConfiguration::
                    read_opt_int(element, literals::Processor::TOTAL_CORES));
            processor.set_enabled_threads(AssetConfiguration::
                    read_opt_int(element, literals::Processor::ENABLED_THREADS));
            processor.set_total_threads(AssetConfiguration::
                    read_opt_int(element, literals::Processor::TOTAL_THREADS));
            std::string type = AssetConfiguration::
                read_string(element ,literals::Processor::TYPE);
            if (enums::ProcessorType::is_allowable_value(type)) {
                processor.set_processor_type(enums::ProcessorType::from_string(type));
            }
            std::string model = AssetConfiguration::read_string(element ,literals::Processor::MODEL);
            if (enums::ProcessorModel::is_allowable_value(model)) {
                processor.set_model(enums::ProcessorModel::from_string(model));
            }
            processor.set_model_name(AssetConfiguration::
                        read_opt_string(element, literals::Processor::MODEL_NAME));
            processor.set_socket(AssetConfiguration::
                    read_string(element, literals::Processor::SOCKET));
            std::string architecture = AssetConfiguration::
                    read_string(element, literals::Processor::ARCHITECTURE);
            if (enums::ProcessorArchitecture::is_allowable_value(architecture)) {
                processor.set_processor_architecture(enums::ProcessorArchitecture::
                        from_string(architecture));
            }
            std::string instruction_set = AssetConfiguration::
                    read_string(element, literals::Processor::INSTRUCTION_SET);
            if (enums::ProcessorInstructionSet::is_allowable_value(instruction_set)) {
                processor.set_instruction_set(enums::ProcessorInstructionSet::
                        from_string(instruction_set));
            }
            processor.set_max_speed_mhz(AssetConfiguration::
                    read_opt_int(element, literals::Processor::MAX_SPEED));
            processor.set_status(make_status(element));
            read_capabilities(processor, element);
            processor.set_cpu_id(make_cpu_id(element));
        }
        return processor;
    }

    void read_capabilities(Processor& processor,
                const xmlpp::Node* element) {
        if (element) {
            for (const auto& capability :
                    element->get_children("capability")) {
                processor.add_capability(AssetConfiguration::get_content(capability));
            }
        }
    }

    attribute::CpuId make_cpu_id(const xmlpp::Node* element) {
        attribute::CpuId cpu_id{};
        if (element) {
            auto elem = element->get_first_child(literals::Processor::CPU_ID);
            if (elem) {
                cpu_id.set_family(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::FAMILY));
                cpu_id.set_model(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::MODEL));
                cpu_id.set_step(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::STEP));
                cpu_id.set_microcode_info(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::MICROCODE_INFO));
                cpu_id.set_vendor_id(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::VENDOR_ID));
                cpu_id.set_numeric_id(AssetConfiguration::
                        read_opt_string(elem, literals::CpuId::NUMERIC_ID));
            }
        }
        return cpu_id;
    }

    void read_usb_devices(System& system, const xmlpp::Node* element) {
        if (element) {
            for (const auto& device : element->get_children(literals::UsbDevice::DEVICE)) {
                system.add_usb_device(UsbDevice{
                        AssetConfiguration::
                            read_string(device, literals::UsbDevice::VENDOR_ID),
                        AssetConfiguration::
                            read_string(device, literals::UsbDevice::DEVICE_ID)
                            });
            }
        }
    }

    void read_pci_devices(System& system, const xmlpp::Node* element) {
        if (element) {
            for (const auto& device : element->get_children(literals::PciDevice::DEVICE)) {
                system.add_pci_device(PciDevice{
                       AssetConfiguration::
                       read_string(device, literals::PciDevice::VENDOR_ID),
                       AssetConfiguration::
                       read_string(device, literals::PciDevice::DEVICE_ID)
                       });
            }
        }
    }

};

bool ComputeLoader::load(const xmlpp::Node* element) {
    if (element) {
        try {
            LoadModules lm{};
            lm.read_asset(element);
        } catch (const xmlpp::exception& e) {
            log_error(GET_LOGGER("discovery"),
                    "Loading modules configuration failed: " << e.what());
            return false;
        }
        return true;
    }
    return false;
}
