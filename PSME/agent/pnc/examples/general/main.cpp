/*!
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
 * @file GAS example
 * @brief GAS access example application.
 * */

#include "gas/mrpc/twi_access_write.hpp"
#include "gas/mrpc/twi_access_read.hpp"
#include "gas/mrpc/partition_binding_info.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "gas/mrpc/bind_port.hpp"
#include "nvme/vital_product_data.hpp"
#include "gas/global_address_space_registers.hpp"
#include "seeprom.hpp"
#include "cable_id.hpp"
#include "i2c/i2c_access_interface_factory.hpp"

using namespace agent::pnc;
using namespace agent::pnc::nvme;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::gas::top;
using namespace agent::pnc::gas::partition;
using namespace agent::pnc::i2c;

int main() {

    size_t twi_attempts = 5;
    size_t partitions = 48;
    size_t ports = 48;

    auto platform = agent_framework::model::enums::PlatformType::EDK;

    // Initialize GAS and read top
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(
        "/sys/bus/pci/devices/0000:05:00.1/resource0");
    gas.read_top();

    I2cAccessInterfaceFactory::get_instance().switch_to_gas();
    I2cAccessInterfaceFactory::get_instance().init_gas_interface("/sys/bus/pci/devices/0000:05:00.1/resource0");

    // Partitions Configuration
    for (uint8_t id = 0; id < gas.top.output.fields.part_num; id++) {
        gas.read_partition(id);
    }

    for(size_t i = 0; i < twi_attempts; i++) {
        /******* TWI Access write *******/
        std::uint8_t seeprom_table_rev[2] = {7, 6};

        TwiAccessWrite cmd1{gas.get_interface()};
        cmd1.input.fields.twi_port = PM85X6TwiPort::PORT0;
        cmd1.input.fields.slave_addr = PM85X6TwiDeviceAddress::SEEPROM;
        cmd1.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
        cmd1.input.fields.offset = 0x00000008;
        cmd1.input.fields.offset_size = TwiOffsetSize::OFFSET_16_BIT;
        cmd1.input.fields.bytes_num = 2;
        cmd1.input.fields.data = seeprom_table_rev;

        if (CommandStatus::DONE != gas.execute_cmd(cmd1)) { }
        if (TwiAccessReturnValue::SUCCESS != cmd1.output.fields.ret_value) { }

        /******** TWI Access read *******/
        TwiAccessRead cmd2{gas.get_interface()};
        cmd2.input.fields.twi_port = PM85X6TwiPort::PORT0;
        cmd2.input.fields.slave_addr = PM85X6TwiDeviceAddress::SEEPROM;
        cmd2.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
        cmd2.input.fields.offset = 0x00000008;
        cmd2.input.fields.offset_size = TwiOffsetSize::OFFSET_16_BIT;
        cmd2.input.fields.bytes_num = 2;

        if (CommandStatus::DONE != gas.execute_cmd(cmd2)) { }
        if (TwiAccessReturnValue::SUCCESS != cmd2.output.fields.ret_value) { }
    }

    /******** SEEPROM ********/
    Seeprom seeprom{};
    seeprom.read(I2cAccessInterfaceFactory::get_instance().get_interface());

    /******** Cable Id *******/
    CableId cable_id(platform);
    try {
        cable_id.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
            PM85X6TwiPort::PORT4, PCA9548TwiExpanderChannel::CHANNEL5);
    }
    catch (const std::exception&) {
        log_debug("agent", "Cabble is not connected.");
    }

    try {
        cable_id.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
            PM85X6TwiPort::PORT4, PCA9548TwiExpanderChannel::CHANNEL0);
    }
    catch (const std::exception&) {
        log_debug("agent", "Cabble is not connected.");
    }

    /******** Vital Product Data ********/
    VitalProductData vpd(platform);
    try {
        vpd.read(I2cAccessInterfaceFactory::get_instance().get_interface(),
            PM85X6TwiPort::PORT3, PCA9548TwiExpanderChannel::CHANNEL0);
    }
    catch (const std::exception&) {
        log_debug("agent", "Drive is not connected.");
    }

    /******** Partition Binding Info ********/
    for (size_t i = 0; i < partitions; i++) {
        PartitionBindingInfo cmd3{gas.get_interface()};
        cmd3.input.fields.partition_id = std::uint8_t(i);
        if (CommandStatus::DONE != gas.execute_cmd(cmd3)) { }
        if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd3.output.fields.ret_value) { }
    }

    /******** Port Binding Info ********/
    for (size_t i = 0; i < ports; i++) {
        PortBindingInfo cmd4{gas.get_interface()};
        cmd4.input.fields.phy_port_id = std::uint8_t(i);
        if (CommandStatus::DONE != gas.execute_cmd(cmd4)) { }
        if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd4.output.fields.ret_value) { }
    }

    /******** Port Binding Info - All enabled ports ********/
    PortBindingInfo cmd5{gas.get_interface()};
    cmd5.input.fields.phy_port_id = 0xff;
    if (CommandStatus::DONE != gas.execute_cmd(cmd5)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd5.output.fields.ret_value) { }


    /******** Port Bind/Unbind *********/

    /*
     * Sequence binds Physical Port 0 from Partition 0,Logical Bridge 1 to Partition 3,Logical Bridge 5
     * !!! THIS CODE MUST BE RUN ONLY ON DEFAULT PMC SWITCH CONFIGURATION !!!
     * */

    // 1) Show Physical Port 0 binding info
    PortBindingInfo cmd6{gas.get_interface()};
    cmd6.input.fields.phy_port_id = 0;
    if (CommandStatus::DONE != gas.execute_cmd(cmd6)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd6.output.fields.ret_value) { }

    // 2) Show Partition 3 binding info
    PartitionBindingInfo cmd7{gas.get_interface()};
    cmd7.input.fields.partition_id = 3;
    if (CommandStatus::DONE != gas.execute_cmd(cmd7)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd7.output.fields.ret_value) { }

    // 3) Unbind Physical Port 0 from Partition 0
    UnbindPort cmd8{gas.get_interface()};
    cmd8.input.fields.partition_id = 0;
    cmd8.input.fields.logical_bridge_id = 1;
    cmd8.input.fields.option = UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_SURPRISE_HOT_REMOVE;
    if (CommandStatus::DONE != gas.execute_cmd(cmd8)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd8.output.fields.ret_value) { }

    // 4) Show Physical Port 0 binding info
    PortBindingInfo cmd9{gas.get_interface()};
    cmd6.input.fields.phy_port_id = 0;
    if (CommandStatus::DONE != gas.execute_cmd(cmd9)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd9.output.fields.ret_value) { }

    // 5) Bind Physical Port 0 to Partition 3, Logical Bridge 5
    BindPort cmd10{gas.get_interface()};
    cmd10.input.fields.phy_port_id = 0;
    cmd10.input.fields.partition_id = 3;
    cmd10.input.fields.logical_bridge_id = 5;
    if (CommandStatus::DONE != gas.execute_cmd(cmd10)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd10.output.fields.ret_value) { }

    // 6) Show Physical Port 0 binding info
    PortBindingInfo cmd11{gas.get_interface()};
    cmd11.input.fields.phy_port_id = 0;
    if (CommandStatus::DONE != gas.execute_cmd(cmd11)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd11.output.fields.ret_value) { }

    // 7) Show Partition 3 binding info
    PartitionBindingInfo cmd12{gas.get_interface()};
    cmd12.input.fields.partition_id = 3;
    if (CommandStatus::DONE != gas.execute_cmd(cmd12)) { }
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd12.output.fields.ret_value) { }

    return 0;
}
