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
 * @file bind_port.cpp
 * @brief Bind Port Command implementation
 * */

#include "logger/logger_factory.hpp"

#include "gas/mrpc/bind_port.hpp"
#include "gas/global_address_space_registers.hpp"



using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;


BindPort::~BindPort() {}


void BindPort::write_input() {

    std::uint8_t data[PORT_PARTITION_P2P_BIND_INPUT_MEMORY_SIZE];

    data[OFFSET_SUB_COMMAND] = std::uint8_t(sub_command);
    data[OFFSET_PARTITION_ID] = input.fields.partition_id;
    data[OFFSET_LOGICAL_BRIDGE_ID] = input.fields.logical_bridge_id;
    data[OFFSET_PHY_PORT_ID] = input.fields.phy_port_id;

    log_debug("pnc-mrpc", "BindPort command input data:"
        << " PartitionId=" << std::uint32_t(input.fields.partition_id)
        << ", LogicalBridgeId=" << std::uint32_t(input.fields.logical_bridge_id)
        << ", PhysicalPortId=" << std::uint32_t(input.fields.phy_port_id));

    m_iface->write(data, PORT_PARTITION_P2P_BIND_INPUT_MEMORY_SIZE, MRPC_INPUT_DATA_REG_OFFSET);
}


void BindPort::read_output() {

    std::uint8_t ret_value[MRPC_COMMAND_RETURN_VALUE_REG_SIZE];

    m_iface->read(ret_value, MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    output.fields.ret_value = PortPartitionP2PBindingReturnValue(
        ret_value[0] | (ret_value[1] << 8) | (ret_value[2] << 16) | (ret_value[3] << 24));

    log_debug("pnc-mrpc", "BindPort command return code: "
        << get_p2p_binding_command_result(static_cast<uint32_t>(output.fields.ret_value)));
}

