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
 * @file twi_access_write.cpp
 * @brief TWI Access Write Command implementation
 * */

#include "safe-string/safe_lib.hpp"
#include "logger/logger_factory.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/twi_access_write.hpp"

using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;

TwiAccessWrite::~TwiAccessWrite(){}

void TwiAccessWrite::write_input() {
    std::uint8_t data[TWI_ACCESS_WRITE_COMMAND_INPUT_MEMORY_SIZE];
    std::stringstream data_str {""};

    if (input.fields.bytes_num > TWI_WRITE_DATA_MAX_SIZE) {
        throw std::runtime_error("Write data is too large.");
    }

    data[OFFSET_SUB_COMMAND] = std::uint8_t(sub_command);
    data[OFFSET_TWI_PORT] = input.fields.twi_port;
    data[OFFSET_SLAVE_ADDRESS] = std::uint8_t(input.fields.slave_addr & 0xff);
    data[OFFSET_SLAVE_ADDRESS + 1] = std::uint8_t((input.fields.slave_addr >> 8) & 0xff);
    data[OFFSET_WRITE_OFFSET] = std::uint8_t(input.fields.offset & 0xff);
    data[OFFSET_WRITE_OFFSET + 1] = std::uint8_t((input.fields.offset >> 8) & 0xff);
    data[OFFSET_WRITE_OFFSET + 2] = std::uint8_t((input.fields.offset >> 16) & 0xff);
    data[OFFSET_WRITE_OFFSET + 3] = std::uint8_t((input.fields.offset >> 24) & 0xff);
    data[OFFSET_BYTES_NUMBER_OFFSET] = std::uint8_t(input.fields.bytes_num & 0xff);
    data[OFFSET_BYTES_NUMBER_OFFSET + 1] = std::uint8_t((input.fields.bytes_num >> 8) & 0xff);
    data[OFFSET_SLAVE_ADDRESS_SIZE] = std::uint8_t(input.fields.slave_addr_size);
    data[OFFSET_WRITE_OFFSET_SIZE] = std::uint8_t(input.fields.offset_size);

    memcpy_s(&data[OFFSET_WRITE_DATA], input.fields.bytes_num, input.fields.data, input.fields.bytes_num);

    for(size_t byte = 0; byte < input.fields.bytes_num; byte++) {
        data_str << "0x" << std::hex << std::uint32_t(input.fields.data[byte]) << " ";
    }

    log_debug("pnc-twi", "TwiAccessWrite command input data:"
                                   << " SlaveAddress=0x" << std::hex << std::uint32_t(input.fields.slave_addr)
                                   << " TwiPort=0x" << std::hex << std::uint32_t(input.fields.twi_port)
                                   << " Offset=0x" << std::hex << std::uint32_t(input.fields.offset)
                                   << " Data=" << data_str.str());

    m_iface->write(data, TWI_ACCESS_WRITE_COMMAND_INPUT_MEMORY_SIZE - TWI_WRITE_DATA_MAX_SIZE
                        + input.fields.bytes_num, MRPC_INPUT_DATA_REG_OFFSET);
}

void TwiAccessWrite::read_output() {
    std::uint8_t ret_value[MRPC_COMMAND_RETURN_VALUE_REG_SIZE];

    m_iface->read(ret_value, MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    output.fields.ret_value = TwiAccessReturnValue(
            ret_value[0] | (ret_value[1] << 8) | (ret_value[2] << 16) | (ret_value[3] << 24));

    log_debug("pnc-twi", "TwiAccessWrite command output data:"
                                   << " ReturnValue=0x" << std::hex << std::uint32_t(output.fields.ret_value));
}

