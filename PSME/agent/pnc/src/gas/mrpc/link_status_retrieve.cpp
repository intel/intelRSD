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
 * @file link_status_retrieve.cpp
 * @brief Link Status Retrieve Command implementation
 * */



#include "logger/logger_factory.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"
#include "gas/global_address_space_registers.hpp"
#include "safe-string/safe_lib.hpp"

using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;

LinkStatusRetrieve::~LinkStatusRetrieve(){}

void LinkStatusRetrieve::write_input() {

    std::uint8_t data[LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE];

    memcpy_s(data, LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE,
                  &input.fields.input_data, LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE);

    log_debug("pnc-link-status", "LinkStatusRetrieve command input data:"
                                   << " InputData=" << std::hex << std::uint64_t(input.fields.input_data));

    m_iface->write(data, LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE, MRPC_INPUT_DATA_REG_OFFSET);
}

void LinkStatusRetrieve::read_output() {

    std::uint8_t ret_value[MRPC_COMMAND_RETURN_VALUE_REG_SIZE];

    m_iface->read(ret_value, MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    output.fields.ret_value = LinkStatusRetrieveReturnValue(
            ret_value[0] | (ret_value[1] << 8) | (ret_value[2] << 16) | (ret_value[3] << 24));

    if (LinkStatusRetrieveReturnValue::COMMAND_SUCCEED == output.fields.ret_value) {
        m_iface->read(reinterpret_cast<uint8_t*>(&output.fields.port_id),
                      LINK_STATUS_RETRIEVE_OUTPUT_MEMORY_SIZE * sizeof(std::uint8_t), MRPC_OUTPUT_DATA_REG_OFFSET);

        log_debug("pnc-link-status", "LinkStatusRetrieve command output data:"
                                       << " ReturnValue=0x" << std::hex << std::uint32_t(output.fields.ret_value) << std::dec
                                       << " PhysicalPortId=" << std::uint32_t(output.fields.port_id)
                                       << " PartitionId=" << std::uint32_t(output.fields.partition_id)
                                       << " LogicalBridgeId=" << std::uint32_t(output.fields.logical_bridge_id)
                                       << " StackId=" << std::uint32_t((output.fields.stack_id_port_id & 0xf0) >> 4)
                                       << " StackPortId=" << std::uint32_t(output.fields.stack_id_port_id & 0xf0)
                                       << " ConfiguredLinkWidth=" << std::uint32_t(output.fields.cfg_link_width)
                                       << " NegotiatedLinkWidth=" << std::uint32_t(output.fields.neg_link_width)
                                       << " UpstreamFlag=" << std::uint32_t(output.fields.ups_flag)
                                       << " LinkUp=" << std::uint32_t((output.fields.linkup_link_rate & 0x80) >> 7)
                                       << " LinkRate=" << std::uint32_t(output.fields.linkup_link_rate & 0x7f)
                                       << " LtssmMajor=" << std::uint32_t(output.fields.major)
                                       << " LtssmMinor=" << std::uint32_t(output.fields.minor));
    }
}
