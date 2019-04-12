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

#include "gas/mrpc/partition_binding_info.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "gas/mrpc/bind_port.hpp"
#include "gas/global_address_space_registers.hpp"
#include "sysfs/sysfs_reader_interface.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "sysfs/sysfs_reader.hpp"
#include <thread>



using namespace agent::pnc;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::gas::top;
using namespace agent::pnc::gas::partition;

namespace {
PartitionBindingInfo get_partition_binding_info(const GlobalAddressSpaceRegisters& gas, uint8_t zone_id) {
    PartitionBindingInfo binding_info{gas.get_interface()};
    binding_info.input.fields.partition_id = zone_id;
    if (CommandStatus::DONE != gas.execute_cmd(binding_info)
        || PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != binding_info.output.fields.ret_value) {
        throw std::runtime_error("Cannot execute PartitionBindingInfo command.");
    }
    return binding_info;
}


uint8_t get_available_bridge(const PartitionBindingInfo& binding_info) {
    uint8_t bridge_count = binding_info.output.fields.logical_bridge_count;

    for (size_t bridge_id = 1; bridge_id < bridge_count; bridge_id++) {
        if (0xff == binding_info.output.fields.partition_binding_info[bridge_id].phy_port_id) {
            return uint8_t(bridge_id);
        }
    }
    throw std::runtime_error("No logical bridge available");
}
}


/*!
 * The program detects PCIe Switch connected the host and binds all DSP (downstream ports) to current partition
 * */
int main() {
    const size_t DSP_PORT_NUMBER = 8;
    SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});
    /* Find all pcie switches connected to the OS */
    auto sysfs_switches = decoder.get_switches();
    if (sysfs_switches.size() > 0) {
        /* Currently supporting only one pcie switch */
        SysfsSwitch sysfs_switch = sysfs_switches.front();
        /* Initialize GAS */
        GlobalAddressSpaceRegisters gas =
            GlobalAddressSpaceRegisters::get_default(sysfs_switch.memory_resource);
        /* Read top level registers */
        gas.read_top();
        log_info("example", "Binding all drives to mgmt partition");
        for (size_t i = 0; i < DSP_PORT_NUMBER; i++) {
            BindPort cmd{gas.get_interface()};
            /* Get number of current partition */
            PartitionBindingInfo partition_binding_info = get_partition_binding_info(gas,
                                                                                     gas.top.output.fields.current_partition_id);
            cmd.input.fields.partition_id = gas.top.output.fields.current_partition_id;
            /* Find available logical bridge for DSP port in current partition */
            cmd.input.fields.logical_bridge_id = get_available_bridge(partition_binding_info);
            // Given the ports are configured to be x4 lanes, there are 8 DSP ports numbered as 0, 2, 4 etc. to 14,
            cmd.input.fields.phy_port_id = uint8_t(2 * i);
            /* Bind DSP to current partition */
            if (CommandStatus::DONE != gas.execute_cmd(cmd)) {}
            if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd.output.fields.ret_value) {}

        }
    }

    return 0;
}
