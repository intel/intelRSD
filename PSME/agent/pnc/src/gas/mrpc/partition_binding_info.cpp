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
 * @file partition_binding_info.cpp
 * @brief Partition Binding Info Command implementation
 * */

#include "logger/logger_factory.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/partition_binding_info.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;


PartitionBindingInfo::~PartitionBindingInfo() {}


void PartitionBindingInfo::write_input() {
    std::uint8_t data[PARTITION_BINDING_INFO_INPUT_MEMORY_SIZE];

    data[OFFSET_SUB_COMMAND] = std::uint8_t(sub_command);
    data[OFFSET_PARTITION_ID] = input.fields.partition_id;

    log_debug("pnc-mrpc", "PartitionBindingInfo command input data:"
        << " PartitionID=" << std::uint32_t(input.fields.partition_id));

    m_iface->write(data, PARTITION_BINDING_INFO_INPUT_MEMORY_SIZE, MRPC_INPUT_DATA_REG_OFFSET);
}


void PartitionBindingInfo::read_output() {
    std::uint8_t ret_value[MRPC_COMMAND_RETURN_VALUE_REG_SIZE];
    std::stringstream msg{""};

    m_iface->read(ret_value, MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);
    output.fields.ret_value = PortPartitionP2PBindingReturnValue(
        ret_value[0] | (ret_value[1] << 8) | (ret_value[2] << 16) | (ret_value[3] << 24));

    /*
     * Partition Binding Info memory map:
     * 0x0000: |   Partition ID   | Logical Bridge Count |     Reserved     |   Reserved   |
     * 0x0004: | Physical Port ID |      BindingState    | Physical Port ID | BindingState |
     * 0x0008: ...
     */
    if (PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED == output.fields.ret_value) {
        m_iface->read(&output.fields.partition_id, 1, MRPC_OUTPUT_DATA_REG_OFFSET);
        m_iface->read(&output.fields.logical_bridge_count, 1, MRPC_OUTPUT_DATA_REG_OFFSET + 1);
        m_iface->read(reinterpret_cast<uint8_t*>(&output.fields.partition_binding_info),
                      output.fields.logical_bridge_count * PARTITION_BINDING_ENTRY_SIZE,
                      MRPC_OUTPUT_DATA_REG_OFFSET + 4);
        log_debug("pnc-mrpc", "PartitionBindingInfo command return code: "
            << get_p2p_binding_command_result(static_cast<uint32_t>(output.fields.ret_value)));
        log_debug("pnc-mrpc", "PartitionBindingInfo command output data:"
            << "PartitionID=" << std::uint32_t(output.fields.partition_id)
            << ", LogicalBridgeCount=" << std::uint32_t(output.fields.logical_bridge_count));

        for (size_t entry = 0; entry < output.fields.logical_bridge_count; entry++) {
            log_debug("pnc-mrpc", "\tPhysicalPort="
                << std::uint32_t(output.fields.partition_binding_info[entry].phy_port_id)
                << ", BindingState="
                << (get_binding_state(output.fields.partition_binding_info[entry].state_operation_result))
                << ", OperationResult="
                << get_operation_result(output.fields.partition_binding_info[entry].state_operation_result));
        }
    }

}

