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
 * @file gas_i2c_interface.cpp
 * @brief GAS based I2c interface implementation
 * */

#include "gas/mrpc/twi_access_write.hpp"
#include "gas/mrpc/twi_access_read.hpp"
#include "i2c/gas_i2c_access_interface.hpp"

#include <safe-string/safe_lib.hpp>

using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::i2c;

GasI2cAccessInterface::~GasI2cAccessInterface() {}

void GasI2cAccessInterface::write(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset) const {

    TwiAccessWrite cmd{m_gas.get_interface()};
    cmd.input.fields.twi_port = port;
    cmd.input.fields.slave_addr = address;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset = offset;
    cmd.input.fields.offset_size = is_16bit_offset ? TwiOffsetSize::OFFSET_16_BIT : TwiOffsetSize::OFFSET_8_BIT;
    cmd.input.fields.bytes_num = size;
    cmd.input.fields.data = data;

    if (CommandStatus::DONE != m_gas.execute_cmd(cmd)) {
        throw std::runtime_error("Cannot execute GAS TWI write command.");
    }
    if (TwiAccessReturnValue::SUCCESS != cmd.output.fields.ret_value) {
        throw std::runtime_error("TWI writing failed.");
    }

}

void GasI2cAccessInterface::read(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset) const {

    TwiAccessRead cmd{m_gas.get_interface()};
    cmd.input.fields.twi_port = port;
    cmd.input.fields.slave_addr = address;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset = offset;
    cmd.input.fields.offset_size = is_16bit_offset ? TwiOffsetSize::OFFSET_16_BIT : TwiOffsetSize::OFFSET_8_BIT;
    cmd.input.fields.bytes_num = size;

    if (CommandStatus::DONE != m_gas.execute_cmd(cmd)) {
        throw std::runtime_error("Cannot execute GAS TWI read command.");
    }

    if (TwiAccessReturnValue::SUCCESS != cmd.output.fields.ret_value) {
        throw std::runtime_error("TWI reading failed.");
    }

    auto res = memcpy_s(data, size, cmd.output.fields.data, size);
    if (EOK != res) {
        throw std::runtime_error(std::string{"Cannot access TWI read result: "} + strerror(res));
    }
}
