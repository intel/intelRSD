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
using namespace agent::pnc::gas;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::gas::partition;


/*
 * The program detects PCIe Switch connected the host and unbinds all DSP (downstream ports) from current partition
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
        gas.read_top();
        log_info("example", "Unbinding all drives from mgmt partition");
        for (size_t i = 0; i < DSP_PORT_NUMBER; i++) {
            UnbindPort cmd{gas.get_interface()};
            cmd.input.fields.partition_id = gas.top.output.fields.current_partition_id;
            /* Logical bridges for DSP start from 1, there are 8 such bridges in the partition*/
            cmd.input.fields.logical_bridge_id = uint8_t(i + 1);
            cmd.input.fields.option = UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_MANAGED_HOT_REMOVE;
            /* Unbind DSP from current partition */
            if (CommandStatus::DONE != gas.execute_cmd(cmd)) {}
            if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED != cmd.output.fields.ret_value) {}
        }
    }
    return 0;
}
