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
 * @file gas_tool.cpp
 *
 * @brief GasTool implementation
 * */

#include "discovery/discovery_utils.hpp"
#include "tools/gas_tool.hpp"
#include "tools/model_tool.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "i2c/gas_i2c_access_interface.hpp"

#include <chrono>
#include <thread>
#include <bitset>
#include <gas/mrpc/partition_binding_info.hpp>
#include <gas/mrpc/port_binding_info.hpp>



using namespace agent::pnc::tools;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::sysfs;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {

constexpr unsigned UNBIND_DEVICE_WAIT_TIME = 5;


uint8_t reverse_byte(uint8_t lsb_byte) {
    std::bitset<8> lsb_bitset = std::bitset<8>(lsb_byte);
    std::bitset<8> msb_bitset{0};
    for (size_t i = 0; i < 8; ++i) {
        msb_bitset[7 - i] = lsb_bitset[i];
    }
    return uint8_t(msb_bitset.to_ulong());
}

}


GasTool::~GasTool() {}


bool GasTool::read_csr_for_usp_port(std::uint8_t partition_id, GlobalAddressSpaceRegisters& gas) const {
    try {
        // get partition configuration
        gas.read_partition(partition_id);

        // check if usp function exist
        if (gas.partition.INVALID_INST_ID == gas.partition.output.fields.usp_inst_id) {
            throw std::runtime_error("No logical bridge for the upstream port");
        }

        // read usp function csr
        const std::uint8_t function_id = static_cast<std::uint8_t>(gas.partition.output.fields.usp_inst_id);
        gas.read_csr(function_id);
        return true;
    }
    catch (const std::exception& e) {
        log_error("gas-tool", "Cannot read port's PCI configuration space: " << e.what());
    }
    return false;
}


PortBindingInfo GasTool::get_port_binding_info(const GlobalAddressSpaceRegisters& gas, uint8_t physical_id) const {
    PortBindingInfo binding_info{gas.get_interface()};
    binding_info.input.fields.phy_port_id = physical_id;
    auto status = gas.execute_cmd(binding_info);
    if (status != CommandStatus::DONE) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute PartitionBindingInfo command. MRPC status code: " + std::to_string(uint32_t(status)));
    }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != binding_info.output.fields.ret_value) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute PortBindingInfo command. Command status code: "
              + get_p2p_binding_command_result(static_cast<uint32_t>(binding_info.output.fields.ret_value)));
    }
    return binding_info;
}


PartitionBindingInfo GasTool::get_partition_binding_info(const GlobalAddressSpaceRegisters& gas,
                                                         uint8_t partition_id) const {
    PartitionBindingInfo binding_info{gas.get_interface()};
    binding_info.input.fields.partition_id = partition_id;
    auto status = gas.execute_cmd(binding_info);
    if (status != CommandStatus::DONE) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute PartitionBindingInfo command. MRPC status code: " + std::to_string(uint32_t(status)));
    }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != binding_info.output.fields.ret_value) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute PartitionBindingInfo command. Command status code: "
              + get_p2p_binding_command_result(static_cast<uint32_t>(binding_info.output.fields.ret_value)));
    }
    return binding_info;
}


std::uint8_t GasTool::bridge_num_to_phys_port_id(const GlobalAddressSpaceRegisters& gas,
                                                 const std::uint8_t bridge_num) const {

    // get number of ports in partition (total number of DSP & USP)
    const std::uint32_t ports_per_partition = gas.partition.output.fields.ports_number;

    // -1 since we do not count USP port
    if (bridge_num >= ports_per_partition - 1) {
        throw std::runtime_error("Bridge number out of range");
    }

    // get 'current partition' (Management partition)
    const std::uint8_t current_partition = gas.top.output.fields.current_partition_id;
    auto binding_info = this->get_partition_binding_info(gas, current_partition);

    return binding_info.output.fields.partition_binding_info[bridge_num + 1].phy_port_id;
}


LinkStatusRetrieve GasTool::get_link_status(const GlobalAddressSpaceRegisters& gas, std::uint8_t phys_port_id) const {
    if (phys_port_id >= PM85X6_PHY_PORTS_NUMBER) {
        log_debug("gas-tool", "Invalid physical port id = " << unsigned(phys_port_id));
        throw std::runtime_error("Incorrect PCIe port Id");
    }
    LinkStatusRetrieve cmd{gas.get_interface()};

    cmd.input.fields.input_data = std::uint64_t(std::uint64_t(1u) << std::uint64_t(phys_port_id));
    if (CommandStatus::DONE != gas.execute_cmd(cmd) ||
        LinkStatusRetrieveReturnValue::COMMAND_SUCCEED != cmd.output.fields.ret_value) {
        throw std::runtime_error("Cannot get PCIe Port Link status.");
    }
    log_debug("gas-tool", "PortLinkStatus for port " << std::uint32_t(phys_port_id)
                                                     << " = " << std::uint32_t(cmd.output.fields.major));
    return cmd;
}


uint8_t GasTool::get_logical_bridge_for_port(const PortBindingInfo& pbi) const {

    if (pbi.output.fields.info_count != 1) {
        log_debug("gas-tool", "Port bound to " << unsigned(pbi.output.fields.info_count)
                                               << " partitions, expected 1");
        throw std::runtime_error("Incorrect port binding info");
    }
    if (0xFF == pbi.output.fields.port_binding_info[0].partition_id ||
        0xFF == pbi.output.fields.port_binding_info[0].logical_bridge_id) {
        log_debug("gas-tool",
                  "Port bound to " << unsigned(pbi.output.fields.port_binding_info[0].partition_id)
                                   << " partition on a bridge "
                                   << unsigned(pbi.output.fields.port_binding_info[0].logical_bridge_id));
        throw std::runtime_error("Incorrect port binding info");
    }
    return pbi.output.fields.port_binding_info[0].logical_bridge_id;
}


void GasTool::bind_to_partition(const GlobalAddressSpaceRegisters& gas, uint8_t phys_port_id, uint8_t partition_id,
                                uint8_t bridge_id) const {
    BindPort cmd{gas.get_interface()};
    cmd.input.fields.logical_bridge_id = bridge_id;
    cmd.input.fields.partition_id = partition_id;
    cmd.input.fields.phy_port_id = phys_port_id;

    auto status = gas.execute_cmd(cmd);
    if (status != CommandStatus::DONE) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot bind function to partition. MRPC status code: " + std::to_string(uint32_t(status)));
    }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd.output.fields.ret_value) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot bind function to partition. Command status code: "
              + get_p2p_binding_command_result(static_cast<uint32_t>(cmd.output.fields.ret_value)));
    }
    check_port_binding_result(gas, partition_id, bridge_id);
}


void GasTool::unbind_management_host_driver(const std::string& switch_bridge_path, uint8_t bridge_id) const {

    SysfsReader reader{};
    SysfsDecoder decoder{};
    decoder.read_sysfs_data(reader);
    // -1 since bridge numbering is different in the switch and in the sysfs
    if (bridge_id == 0) {
        THROW(agent_framework::exceptions::PncError, "agent", "Invalid bridge id while unbinding device driver.");
    }
    auto bridge = decoder.get_bridge_by_switch_path(switch_bridge_path, uint8_t(bridge_id - 1));
    auto devices = decoder.get_devices(bridge);
    for (const auto& device : devices) {
        auto functions = decoder.get_functions(device);
        for (const auto& function : functions) {
            if (agent::pnc::discovery::utils::to_device_class_enum(function.device_class) ==
                enums::PcieFunctionDeviceClass::MassStorageController) {
                if (!reader.unbind_nvme_driver(function.id)) {
                    THROW(agent_framework::exceptions::PncError, "agent", "Cannot unbind device's driver");
                }
            }
        }
    }
}


void GasTool::unbind_from_partition(const GlobalAddressSpaceRegisters& gas, uint8_t partition_id,
                                    uint8_t bridge_id) const {

    auto partition_info = get_partition_binding_info(gas, partition_id);
    auto port_id = partition_info.output.fields.partition_binding_info[bridge_id].phy_port_id;
    UnbindPort unbind_cmd{gas.get_interface()};
    unbind_cmd.input.fields.partition_id = partition_id;
    unbind_cmd.input.fields.logical_bridge_id = bridge_id;
    unbind_cmd.input.fields.option = UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_SURPRISE_HOT_REMOVE;

    // Checking if MRPC status is DONE. IN_PROGRESS/IDLE/ERROR status handle in execute_cmd.
    auto gas_ret = gas.execute_cmd(unbind_cmd);
    if (CommandStatus::DONE != gas_ret) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute UnbindPort command. MRPC status code: " + std::to_string(uint32_t(gas_ret)));
    }

    // Here we are sure that MRPC returned DONE. We can check command status.
    auto cmd_ret = unbind_cmd.output.fields.ret_value;
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd_ret) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot execute UnbindPort command. Command status code: "
              + get_p2p_binding_command_result(uint32_t(cmd_ret)));
    }

    check_port_unbinding_result(gas, port_id);
}


void GasTool::check_port_unbinding_result(const gas::GlobalAddressSpaceRegisters& gas, uint8_t phys_port_id) const {
    auto port_info = get_port_binding_info(gas, phys_port_id);
    auto result = OperationResult::IN_PROGRESS;

    do {
        port_info = get_port_binding_info(gas, phys_port_id);
        result = get_operation_result_enum(
            port_info.output.fields.port_binding_info[0].state_operation_result);
        log_debug("gas-tool",
                  "Unbinding command result " << get_operation_result(static_cast<uint8_t>(result)));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    while (OperationResult::IN_PROGRESS == result);
    if (OperationResult::SUCCESS != result) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Unbinding port failed. Command result " + get_operation_result(static_cast<uint8_t>(result)));
    }
    log_debug("gas-tool",
              "Unbinding command result " << get_operation_result(static_cast<uint8_t>(result)));
}


void GasTool::check_port_binding_result(const gas::GlobalAddressSpaceRegisters& gas, uint8_t partition_id,
                                        uint8_t bridge_id) const {

    auto partition_info = get_partition_binding_info(gas, partition_id);
    auto result = OperationResult::IN_PROGRESS;
    do {
        partition_info = get_partition_binding_info(gas, partition_id);
        result = get_operation_result_enum(
            partition_info.output.fields.partition_binding_info[bridge_id].state_operation_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    while (OperationResult::IN_PROGRESS == result);
    if (OperationResult::SUCCESS != result) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Binding port failed. Command result : " + get_operation_result(static_cast<uint8_t>(result)));
    }
    log_debug("gas-tool", "Binding result " << get_operation_result(static_cast<uint8_t>(result)));
}


uint8_t GasTool::get_available_bridge_id(const PartitionBindingInfo& pbi) const {
    uint8_t bridge_count = pbi.output.fields.logical_bridge_count;
    for (size_t bridge_id = 1; bridge_id < bridge_count; bridge_id++) {
        if (NO_PORT_ASSIGNED == pbi.output.fields.partition_binding_info[bridge_id].phy_port_id) {
            return uint8_t(bridge_id);
        }
    }
    THROW(agent_framework::exceptions::PncError, "agent", "No logical bridge available in the partition.");
}


uint8_t GasTool::get_available_bridges_count(const PartitionBindingInfo& pbi) const {
    uint8_t available_bridges = 0;
    uint8_t bridge_count = pbi.output.fields.logical_bridge_count;
    for (size_t bridge_id = 1; bridge_id < bridge_count; bridge_id++) {
        if (NO_PORT_ASSIGNED == pbi.output.fields.partition_binding_info[bridge_id].phy_port_id) {
            available_bridges++;
        }
    }
    return available_bridges;
}


uint64_t GasTool::get_presence_bitmask(const GlobalAddressSpaceRegisters& gas) const {
    //TODO: Investigate if it requires change for FPGA
    i2c::GasI2cAccessInterface i2c{gas.get_interface()};
    uint8_t ports_0_14{};
    uint8_t ports_32_46{};
    i2c.read(NVMeDrivePresence::PORTS_0_14, PM85X6TwiDeviceAddress::DRIVE_PRESENCE, 0, &ports_0_14, 1, false);
    i2c.read(NVMeDrivePresence::PORTS_32_46, PM85X6TwiDeviceAddress::DRIVE_PRESENCE, 0, &ports_32_46, 1, false);
    // mask with drive presence is returned in MSB order instead of expected LSB
    uint16_t all_ports = uint16_t(uint16_t(reverse_byte(ports_0_14)) | (uint16_t(reverse_byte(ports_32_46)) << 8));
    uint64_t result = uint64_t(~all_ports);
    return result;
}


void GasTool::bind_endpoint_to_zone(const GlobalAddressSpaceRegisters& gas, const std::string& zone_uuid,
                                    const std::string& endpoint_uuid) const {
    auto zone = get_manager<Zone>().get_entry(zone_uuid);
    OptionalField<std::uint8_t> partition_id = get_partition_id_by_zone(gas, zone.get_uuid());
    if (partition_id.has_value()) {
        log_debug("gas-tool", "Binding Endpoint " << endpoint_uuid << " to Zone " << zone_uuid << "with Partition ID "
                                                  << static_cast<unsigned>(partition_id.value()));
        PartitionBindingInfo part_bi = this->get_partition_binding_info(gas, partition_id);
        uint8_t free_bridges = get_available_bridges_count(part_bi);
        // throw exception if not enough bridges are available
        if (free_bridges < get_m2m_manager<Endpoint, Port>().get_children(endpoint_uuid).size()) {
            THROW(agent_framework::exceptions::PncError, "agent",
                  "Not enough empty bridges available in the partition");
        }
        auto ports = get_m2m_manager<Endpoint, Port>().get_children(endpoint_uuid);
        for (const auto& port_uuid : ports) {
            Port port = get_manager<Port>().get_entry(port_uuid);
            PortBindingInfo port_bi = this->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
            this->bind_to_partition(gas,
                                    uint8_t(port.get_phys_port_id()),
                                    partition_id,
                                    this->get_available_bridge_id(part_bi));
        }
    }
    else {
        log_debug("gas-tool", "Zone with UUID " << zone_uuid << " doesn't have associated partition.");
    }
}


void GasTool::unbind_endpoint_from_zone(const GlobalAddressSpaceRegisters& gas, const std::string& zone_uuid,
                                        const std::string& endpoint_uuid) const {
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    auto ports = get_m2m_manager<Endpoint, Port>().get_children(endpoint_uuid);
    for (const auto& port_uuid : ports) {
        Port port = get_manager<Port>().get_entry(port_uuid);
        PortBindingInfo pbi = this->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
        this->unbind_from_partition(gas, pbi.output.fields.port_binding_info[0].partition_id,
                                    pbi.output.fields.port_binding_info[0].logical_bridge_id);
    }
}


void GasTool::bind_device_to_mgmt_partition(ModelToolPtr mtool, gas::GlobalAddressSpaceRegisters& gas,
                                            const Uuid& device_uuid) const {

    gas.read_top();
    if (!mtool->exists_in_components(device_uuid)) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "PCIe device with UUID = ['" + device_uuid + "'] not found.");
    }

    // we assume only one port will be found
    auto ports = mtool->get_ports_by_device_uuid(device_uuid);
    if (ports.empty()) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot bind PCIe device to mgmt zone: pcie function not found");
    }
    auto port = get_manager<Port>().get_entry(ports[0]);

    PartitionBindingInfo part_bi = this->get_partition_binding_info(gas, gas.top.output.fields.current_partition_id);
    uint8_t bridge_id = this->get_available_bridge_id(part_bi);

    PortBindingInfo port_bi = this->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
    if (NO_PORT_ASSIGNED == port_bi.output.fields.port_binding_info->partition_id) {
        this->bind_to_partition(gas, uint8_t(port.get_phys_port_id()),
                                gas.top.output.fields.current_partition_id, bridge_id);
    }
    else {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot bind device to mgmt zone. Port already bound to partition "
              + std::to_string(int(port_bi.output.fields.port_binding_info->partition_id)));
    }
}


void GasTool::unbind_device_from_mgmt_partition(ModelToolPtr mtool, GlobalAddressSpaceRegisters& gas,
                                                const std::string& device_uuid) const {

    gas.read_top();
    if (!mtool->exists_in_components(device_uuid)) {

        THROW(agent_framework::exceptions::PncError, "agent",
              "Device with UUID = ['" + device_uuid + "'] not found.");
    }

    // we assume only one port will be found
    auto ports = mtool->get_ports_by_device_uuid(device_uuid);
    if (ports.empty()) {
        THROW(agent_framework::exceptions::PncError, "agent",
              "Cannot unbind PCIe device from mgmt zone: pcie function not found");
    }
    auto port = get_manager<Port>().get_entry(ports[0]);

    PortBindingInfo pbi = this->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
    if (NO_PORT_ASSIGNED != pbi.output.fields.port_binding_info->partition_id) {
        this->unbind_from_partition(gas, pbi.output.fields.port_binding_info->partition_id,
                                    pbi.output.fields.port_binding_info->logical_bridge_id);
        std::this_thread::sleep_for(std::chrono::seconds(UNBIND_DEVICE_WAIT_TIME));
    }
}


OptionalField<std::uint8_t> GasTool::get_partition_id_by_zone(const gas::GlobalAddressSpaceRegisters& gas,
                                                              const Uuid& zone_uuid) const {
    // Path to get partition ID:
    // Zone -> Initiator Endpoint -> Port -> Port physical ID -> GAS Port binding -> Partition ID

    if(!get_manager<Zone>().entry_exists(zone_uuid)) {
        THROW(agent_framework::exceptions::InvalidUuid, "model-tool", "Zone not found, invalid uuid: " + zone_uuid);
    }

    // Get Partition ID for port using phys_port_id
    PortBindingInfo cmd{gas.get_interface()};
    try {
        cmd = get_all_port_binding_info(gas);
    }
    catch (const std::exception& e) {
        log_error("model-tool",
                  "Cannot get port binding info for all ports during getting partition ID for Zone: "
                      << e.what());
        return OptionalField<std::uint8_t>();
    }

    // Get initiator endpoint
    log_debug("model-tool", "Getting Partition ID for Zone with UUID " << zone_uuid);
    for (const auto& endpoint_uuid : get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid)) {
        const auto& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (endpoint.get_connected_entities().size() > 0) {
            const auto& entity_role = endpoint.get_connected_entities()[0].get_entity_role();
            if (entity_role == enums::EntityRole::Initiator) {

                // Get Port for initiator endpoint
                for (const auto& port_uuid : get_m2m_manager<Endpoint, Port>().get_children(endpoint_uuid)) {
                    const auto phys_port_id = get_manager<Port>().get_entry(port_uuid).get_phys_port_id();

                    for (auto& port_binding_into : cmd.output.fields.port_binding_info) {
                        if (port_binding_into.phy_port_id == phys_port_id) {
                            return port_binding_into.partition_id;
                        }
                    }

                }

            }
        }
    }

    return OptionalField<std::uint8_t>();
}


GlobalAddressSpaceRegisters GasTool::get_gas(const Uuid& switch_uuid) {
    auto pcie_switch = get_manager<Switch>().get_entry(switch_uuid);
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(pcie_switch.get_memory_path());
    gas.read_top();
    gas.read_partition(gas.top.output.fields.current_partition_id);
    return gas;
}