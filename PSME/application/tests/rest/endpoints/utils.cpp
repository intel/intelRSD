/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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


#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "utils.hpp"




using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;


void clean_model_data() {
    get_manager<System>().clear_entries();
    get_manager<Processor>().clear_entries();
    get_manager<Manager>().clear_entries();
    get_manager<Chassis>().clear_entries();
    get_manager<Drive>().clear_entries();
    get_manager<Memory>().clear_entries();
    get_manager<NetworkInterface>().clear_entries();
    get_manager<StorageController>().clear_entries();
}


namespace {
std::uint32_t PROCESSOR_SOCKET = 0;


Manager make_manager(const std::string& parent, const std::string& uuid) {
    Manager manager{};

    manager.set_parent_uuid(parent);
    manager.set_uuid(uuid);

    manager.set_status({
                           agent_framework::model::enums::State::Enabled,
                           agent_framework::model::enums::Health::OK
                       });
    manager.add_collection(
        attribute::Collection(enums::CollectionName::Systems, enums::CollectionType::Systems));
    return manager;
}


System make_system(const std::string& parent, const std::string& uuid) {
    System system{};

    system.set_parent_uuid(parent);
    system.set_uuid(uuid);

    system.set_fru_info({"123fed3029c-b23394-12",
                         "Intel Corporation",
                         "E323",
                         "29ee2220939"});
    system.set_bios_version("A20F21_A0");
    system.set_boot_override(enums::BootOverride::Once);
    system.set_boot_override_target(enums::BootOverrideTarget::Hdd);
    system.add_boot_override_supported(enums::BootOverrideTarget::Hdd);
    system.add_boot_override_supported(enums::BootOverrideTarget::Pxe);
    system.add_boot_override_supported(enums::BootOverrideTarget::None);
    system.set_power_state(enums::PowerState::On);
    system.set_status({
                          agent_framework::model::enums::State::Enabled,
                          agent_framework::model::enums::Health::OK
                      });
    system.add_usb_device({"8087", "0024"});
    system.add_usb_device({"1d6b", "0002"});
    system.add_pci_device({"8086", "0023"});
    system.add_pci_device({"1d5b", "0003"});
    system.add_collection(attribute::Collection(
            enums::CollectionName::Processors, enums::CollectionType::Processors
    ));
    system.add_collection(attribute::Collection(
            enums::CollectionName::Memory, enums::CollectionType::Memory
    ));
    system.add_collection(attribute::Collection(
            enums::CollectionName::StorageSubsystems, enums::CollectionType::StorageSubsystems
    ));
    system.add_collection(attribute::Collection(
            enums::CollectionName::NetworkInterfaces, enums::CollectionType::NetworkInterfaces
    ));

    return system;

}


Processor make_processor(const std::string& parent, const std::string& uuid) {
    Processor processor;

    processor.set_parent_uuid(parent);
    processor.set_uuid(uuid);

    processor.set_socket(std::to_string(++PROCESSOR_SOCKET));
    processor.set_processor_type(enums::ProcessorType::CPU);
    processor.set_processor_architecture(enums::ProcessorArchitecture::x86);
    processor.set_instruction_set(enums::ProcessorInstructionSet::x86_64);
    processor.set_manufacturer(literals::Intel::INTEL_CORP);
    processor.set_model_name("Multi-Core Intel(R) Xeon(R) processor 7xxx Series");
    processor.set_model(enums::ProcessorModel::E3);

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
}


void generate_model_data() {

    get_manager<Manager>().add_entry(make_manager("", "1111"));
    get_manager<Manager>().add_entry(make_manager("", "1222"));
    get_manager<Manager>().add_entry(make_manager("", "1333"));

    get_manager<System>().add_entry(make_system("1111", "2111"));
    get_manager<System>().add_entry(make_system("1111", "2222"));
    get_manager<System>().add_entry(make_system("1222", "2333"));

    get_manager<Processor>().add_entry(::make_processor("2111", "3111"));
    get_manager<Processor>().add_entry(::make_processor("2111", "3222"));
    get_manager<Processor>().add_entry(::make_processor("2111", "3333"));
}

