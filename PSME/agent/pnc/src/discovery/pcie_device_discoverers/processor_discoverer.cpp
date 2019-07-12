/*!
 * @brief Implementation of class for discovery of PCIe device represented in Redfish as Processor
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
 * @file processor_discoverer.cpp
 */

#include "protocol/pldm/command/get_terminus_locator_pdr.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/tree_stability/tree_stabilizer.hpp"
#include "discovery/device_discoverers/processor_discoverer.hpp"
#include "discovery/discovery_exceptions.hpp"
#include "discovery/device_discoverers/ib/ib_processor_data_updater.hpp"



using namespace agent::pnc::discovery;
using namespace agent::pnc::discovery::device_discoverers;
using namespace agent::pnc::discovery::device_discoverers::data_updater;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;


agent_framework::model::Processor ProcessorDiscoverer::discover_oob(const Uuid& downstream_port_uuid) const {

    Uuid system_uuid = m_tools.model_tool->get_system_uuid();

    auto builder = m_factory->init_builder(m_factory->get_processor_builder(), system_uuid);

    builder->init(system_uuid);

    Port port{};

    try {
        port = get_manager<Port>().get_entry(downstream_port_uuid);
        builder->add_dsp_port_uuid(downstream_port_uuid);
    }
    catch (...) {
        log_debug("pnc-discovery",
                  "Invalid downstream port uuid: " << downstream_port_uuid << " for the processor: " << system_uuid);
        log_error("pnc-discovery", "Cannot access downstream port of a processor");
    }

    Uuid chassis_uuid = m_tools.model_tool->get_chassis_uuid();
    PlatformType platform{get_manager<Chassis>().get_entry(chassis_uuid).get_platform()};
    agent::pnc::pldm::command::GetTerminusLocatorPdr terminus_locator_pdr(platform);
    // Since we cannot enforce installing Aardvark device on the system, but still need to handle discovered devices
    // in some way - there are two ways of generating fru info. With real data from aardvark, and with the use of pcie
    // switch port.
    if (m_tools.oob_fpga_tool->get_pdr(terminus_locator_pdr, port)) {
        attribute::FruInfo fru_info(
            {terminus_locator_pdr.get_device_uid(), // this is unique, but is not FPGA serial number
             OptionalField<std::string>(),
             OptionalField<std::string>(),
             OptionalField<std::string>()});
        builder->update_fru_info(fru_info);
    }
    else {
        throw agent::pnc::discovery::PncDiscoveryExceptionDeviceNotFound();
    }

    return builder->build();
}


Uuid ProcessorDiscoverer::discover_oob_add_and_stabilize(const Uuid& port_uuid) const {

    auto port = agent_framework::module::get_manager<agent_framework::model::Port>().get_entry(port_uuid);

    Processor processor = discover_oob(port_uuid);

    log_debug("pnc-discovery", "Processor (" << processor.get_processor_type().value().to_string()
                                             << ") has been found on a physical port " << port.get_port_id());

    return stabilize_processor_update_model(processor);
}


void ProcessorDiscoverer::critical_state_device_discovery(const Uuid& device_uuid) const {

    log_debug("pnc-discovery", "Processor is not visible in the sysfs = " << device_uuid);
    log_error("pnc-discovery", "Processor was detected but no pcie devices are present!");

    Uuid system_uuid = m_tools.model_tool->get_system_uuid();

    Processor processor = get_manager<Processor>().get_entry(device_uuid);

    processor = discover_no_sysfs_ib_processor(processor);

    log_and_update(processor);

    // send update event for processor
    agent_framework::eventing::send_update(processor);
}


Processor ProcessorDiscoverer::discover_no_sysfs_ib_processor(const Processor& processor) const {

    return m_factory->init_builder(m_factory->get_processor_builder(), processor)->update_critical_state().build();
}


void ProcessorDiscoverer::set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                                                const Uuid& device_uuid) const {

    if (!device_uuid.empty()) {

        agent_framework::module::get_m2m_manager<agent_framework::model::Processor, PcieFunction>().add_entry(
            device_uuid,
            pcie_function.get_uuid());

        pcie_function.set_functional_device(device_uuid);
    }
}


Uuid ProcessorDiscoverer::sysfs_device_discovery(const Uuid& dsp_port_uuid,
                                                 const Uuid& device_uuid,
                                                 const agent::pnc::sysfs::SysfsDecoder& decoder,
                                                 const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "Begin sysfs device discovery for processor: " << device_uuid);

    Uuid manager_uuid = m_tools.model_tool->get_manager_uuid();
    Uuid chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    // discover pcie device and functions
    PcieDevice pcie_device = BaseClass::discover_pcie_device(manager_uuid, chassis_uuid, sysfs_device);
    SysfsFunctionsVec sysfs_functions = decoder.get_functions(sysfs_device);
    PcieFunctionsVec pcie_functions = get_pcie_functions(pcie_device.get_uuid(), dsp_port_uuid, sysfs_functions);

    if (!is_processing_accelerator_class_device(pcie_functions)) {

        log_warning("pnc-discovery",
                    "Processor was detected but no sysfs processing accelerators "
                    "were discovered or discovered device is not a processor");

        throw PncDiscoveryExceptionDeviceNotFound();
    }

    log_and_add(pcie_device);

    Processor processor{};
    std::string serial_number{};
    Uuid processor_uuid{};

    if (get_manager<Processor>().entry_exists(device_uuid)) {

        processor = get_manager<Processor>().get_entry(device_uuid);
        serial_number = processor.get_fru_info().get_serial_number();
        processor_uuid = device_uuid;
    }
    else {

        serial_number = get_serial_number_based_on_dsp_port(dsp_port_uuid);
        processor_uuid = add_from_sysfs(dsp_port_uuid, serial_number);
        sync_device_properties_with_db(processor_uuid);
    }

    for (auto pcie_function : pcie_functions) {

        set_functional_device(pcie_function, processor_uuid);

        log_and_add(pcie_function);
    }

    const auto& persistent_pcie_device_uuid = ::agent::pnc::PncTreeStabilizer().stabilize_pcie_device<Processor>(
        pcie_device.get_uuid(),
        serial_number);

    agent_framework::eventing::send_event(persistent_pcie_device_uuid,
                                          enums::Component::PcieDevice,
                                          agent_framework::model::enums::Notification::Add,
                                          manager_uuid);

    log_debug("pnc-discovery", "End sysfs device discovery for processor: " << processor_uuid);

    return processor_uuid;
}


void ProcessorDiscoverer::sysfs_discovery(const Uuid& device_uuid,
                                          const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "Processor ib sysfs discovery begin");

    Processor processor = get_manager<Processor>().get_entry(device_uuid);

    processor = discovery_ib_processor(processor, sysfs_device);

    log_and_update(processor);

    // send update event for processor
    agent_framework::eventing::send_event(device_uuid,
                                          enums::Component::Processor,
                                          agent_framework::model::enums::Notification::Update,
                                          processor.get_parent_uuid());

}


Processor ProcessorDiscoverer::discovery_ib_processor(const agent_framework::model::Processor& processor,
                                                      const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "sysfs device id: " << sysfs_device.id.to_string());

    IBProcessorDataUpdater updater(sysfs_device, processor);

    return updater.update_processor_data();
}


void ProcessorDiscoverer::sync_device_properties_with_db(const Uuid& device_uuid) const {
    auto processor = agent_framework::module::get_manager<agent_framework::model::Processor>().get_entry_reference(
        device_uuid);
    m_tools.model_tool->sync_device_with_db<agent_framework::model::Processor, agent_framework::model::System>(
        m_tools.model_tool->get_dry_stabilized_system_uuid(), processor.get_raw_ref());
    log_debug("pnc-discovery", "Processor [UUID = " + device_uuid + "] properties synchronized with database");
}


ProcessorDiscoverer::PcieFunctionsVec ProcessorDiscoverer::get_pcie_functions(const Uuid& device_uuid,
                                                                              const Uuid& dsp_port_uuid,
                                                                              const SysfsFunctionsVec& functions) const {

    PcieFunctionsVec pcie_functions{};

    std::for_each(functions.begin(), functions.end(), [&](const agent::pnc::sysfs::SysfsFunction& function) {
        pcie_functions.push_back(BaseClass::discover_pcie_function(device_uuid, dsp_port_uuid, function));
    });

    return pcie_functions;
}


bool ProcessorDiscoverer::is_processing_accelerator_class_device(const PcieFunctionsVec& pcie_functions) const {

    for (const auto& pcie_function : pcie_functions) {

        if (!pcie_function.get_device_class().has_value()) {
            log_error("pnc-discovery",
                      "Device class not available for given PCIe function: " << pcie_function.get_uuid());
            return false;
        }

        if (pcie_function.get_device_class() != enums::PcieFunctionDeviceClass::ProcessingAccelerators) {
            return false;
        }
    }

    return true;
}


Uuid ProcessorDiscoverer::add_from_sysfs(const Uuid& dsp_port_uuid,
                                         const std::string& serial_number) const {

    log_debug("pnc-discovery", "Add sysfs device: processor");

    Uuid system_uuid = m_tools.model_tool->get_system_uuid();

    auto builder = m_factory->init_builder(m_factory->get_processor_builder(), system_uuid);

    builder->init(system_uuid);
    builder->add_dsp_port_uuid(dsp_port_uuid);

    attribute::FruInfo fru_info({serial_number,
                                 OptionalField<std::string>(),
                                 OptionalField<std::string>(),
                                 OptionalField<std::string>()});

    builder->update_fru_info(fru_info);

    Processor processor = builder->build();

    log_debug("pnc-discovery", "Processor was created based on sysfs data, dsp port uuid: " << dsp_port_uuid);

    return stabilize_processor_update_model(processor);
}


std::string ProcessorDiscoverer::get_serial_number_based_on_dsp_port(const Uuid& dsp_port_uuid) const {

    Port port{};

    if (get_manager<Port>().entry_exists(dsp_port_uuid)) {

        port = get_manager<Port>().get_entry(dsp_port_uuid);
    }
    else {

        log_error("pnc-discovery", "Downstream port not available in the model");

        throw PncDiscoveryExceptionDeviceNotFound();
    }

    log_debug("pnc-discovery", "Serial number based on port number: " << port.get_phys_port_id());

    return std::to_string(port.get_phys_port_id());
}


const Uuid ProcessorDiscoverer::stabilize_processor_update_model(const Processor& processor) const {
    Uuid system_uuid = m_tools.model_tool->get_system_uuid();

    log_and_add(processor);

    auto processor_uuid = ::agent::pnc::PncTreeStabilizer().stabilize_processor(processor.get_uuid());
    agent_framework::eventing::send_event(system_uuid,
                                          enums::Component::System,
                                          agent_framework::model::enums::Notification::Update,
                                          get_manager<System>().get_entry(system_uuid).get_parent_uuid());
    agent_framework::eventing::send_event(processor_uuid,
                                          enums::Component::Processor,
                                          agent_framework::model::enums::Notification::Add,
                                          system_uuid);

    return processor_uuid;
}
