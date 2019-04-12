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
 * @file twi_access_read.cpp
 * @brief TWI Access Read Command implementation
 * */

#include "logger/logger_factory.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/twi_access_read.hpp"

using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;

TwiAccessRead::~TwiAccessRead(){}

void TwiAccessRead::write_input() {

    if (!m_iface) {
        throw std::runtime_error("Access interface is not initialized.");
    }

    std::uint8_t data[TWI_ACCESS_READ_COMMAND_INPUT_MEMORY_SIZE];

    if (input.fields.bytes_num > TWI_READ_DATA_MAX_SIZE) {
        throw std::runtime_error("Requested read data is too large.");
    }

    data[OFFSET_SUB_COMMAND] = std::uint8_t(sub_command);
    data[OFFSET_TWI_PORT] = input.fields.twi_port;
    data[OFFSET_SLAVE_ADDRESS] = std::uint8_t(input.fields.slave_addr & 0xff);
    data[OFFSET_SLAVE_ADDRESS + 1] = std::uint8_t((input.fields.slave_addr >> 8) & 0xff);
    data[OFFSET_READ_OFFSET] = std::uint8_t(input.fields.offset & 0xff);
    data[OFFSET_READ_OFFSET + 1] = std::uint8_t((input.fields.offset >> 8) & 0xff);
    data[OFFSET_READ_OFFSET + 2] = std::uint8_t((input.fields.offset >> 16) & 0xff);
    data[OFFSET_READ_OFFSET + 3] = std::uint8_t((input.fields.offset >> 24) & 0xff);
    data[OFFSET_BYTES_NUMBER_OFFSET] = std::uint8_t(input.fields.bytes_num & 0xff);
    data[OFFSET_BYTES_NUMBER_OFFSET + 1] = std::uint8_t((input.fields.bytes_num >> 8) & 0xff);
    data[OFFSET_SLAVE_ADDRESS_SIZE] = std::uint8_t(input.fields.slave_addr_size);
    data[OFFSET_READ_OFFSET_SIZE] = std::uint8_t(input.fields.offset_size);
    data[OFFSET_READ_DATA] = 0;

    log_debug("pnc-twi", "TwiAccessRead command input data:"
                                   << " SlaveAddress=0x" << std::hex << std::uint32_t(input.fields.slave_addr)
                                   << " TwiPort=0x" << std::hex << std::uint32_t(input.fields.twi_port)
                                   << " Offset=0x" << std::hex << std::uint32_t(input.fields.offset));

    m_iface->write(data, TWI_ACCESS_READ_COMMAND_INPUT_MEMORY_SIZE, MRPC_INPUT_DATA_REG_OFFSET);
}

void TwiAccessRead::read_output() {
    std::uint8_t ret_value[MRPC_COMMAND_RETURN_VALUE_REG_SIZE];
    std::stringstream data_str {""};

    if (!m_iface) {
        throw std::runtime_error("Access interface is not initialized.");
    }

    if (input.fields.bytes_num > TWI_READ_DATA_MAX_SIZE) {
        throw std::runtime_error("Requested read data is too large.");
    }

    m_iface->read(ret_value, MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);
    output.fields.ret_value = TwiAccessReturnValue(
            ret_value[0] | (ret_value[1] << 8) | (ret_value[2] << 16) | (ret_value[3] << 24));

    if (TwiAccessReturnValue::SUCCESS == output.fields.ret_value) {
        m_iface->read(output.fields.data, input.fields.bytes_num, MRPC_OUTPUT_DATA_REG_OFFSET);
        for (size_t byte = 0; byte < input.fields.bytes_num; byte++) {
            data_str << std::hex << std::uint32_t(output.fields.data[byte]) << " ";
        }
    }

    log_debug("pnc-twi", "TwiAccessRead command output data:"
                                    << " ReturnValue=" << std::hex << uint32_t(output.fields.ret_value)
                                    << " Data=" << data_str.str());
}
