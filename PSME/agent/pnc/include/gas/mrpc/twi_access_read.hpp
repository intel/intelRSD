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
 * @file twi_access_read.hpp
 * @brief TWI Access Read Command
 * */

#pragma once

#include "command.hpp"

#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {
/*! MRPC namespace */
namespace mrpc {

/*! TWI Access Read Command */
class TwiAccessRead final : public Command {
public:
    using Command::Command;

    /*! TWI Access Read Command input memory size */
    static constexpr std::uint8_t TWI_ACCESS_READ_COMMAND_INPUT_MEMORY_SIZE = 13;
    /*! TWI Access Read Command output memory size */
    static constexpr std::uint16_t TWI_ACCESS_READ_COMMAND_OUTPUT_MEMORY_SIZE = 1012;
    /*! TWI Max Data size */
    static constexpr std::uint16_t TWI_READ_DATA_MAX_SIZE = 1012;

    /*! SubCommand offset */
    static constexpr std::uint8_t OFFSET_SUB_COMMAND = 0;
    /*! TWI Port offset */
    static constexpr std::uint8_t OFFSET_TWI_PORT = 1;
    /*! TWI Slave Address offset */
    static constexpr std::uint8_t OFFSET_SLAVE_ADDRESS = 2;
    /*! TWI Read Offset offset */
    static constexpr std::uint8_t OFFSET_READ_OFFSET = 4;
    /*! TWI Bytes Number offset */
    static constexpr std::uint8_t OFFSET_BYTES_NUMBER_OFFSET = 8;
    /*! TWI Slave Address Size offset */
    static constexpr std::uint8_t OFFSET_SLAVE_ADDRESS_SIZE = 10;
    /*! TWI Read Offset Size offset */
    static constexpr std::uint8_t OFFSET_READ_OFFSET_SIZE = 11;
    /*! TWI Read Data offset */
    static constexpr std::uint8_t OFFSET_READ_DATA = 12;

    /*! TWI Access read command code */
    TwiAccessSubCommandCode sub_command = TwiAccessSubCommandCode::READ;

    TwiAccessRead(AccessInterface* iface) : Command(iface, CommandCode::TWI_ACCESS) {}

#pragma pack(push,1)
    /*! TWI Access Command input structure */
    union {
        struct {
            std::uint8_t twi_port;
            std::uint16_t slave_addr;
            std::uint32_t offset;
            std::uint16_t bytes_num;
            TwiSlaveAddressSize slave_addr_size;
            TwiOffsetSize offset_size;
        } fields;
        std::uint8_t raw[TWI_ACCESS_READ_COMMAND_INPUT_MEMORY_SIZE];
    } input{};

    /*! TWI Access Command output structure */
    union {
        struct {
            TwiAccessReturnValue ret_value;
            std::uint8_t data[TWI_READ_DATA_MAX_SIZE];
        } fields;
        std::uint8_t raw[TWI_ACCESS_READ_COMMAND_OUTPUT_MEMORY_SIZE];
    } output{};
#pragma pack(pop)

    /*! Write input data */
    void write_input();

    /*! Read output data */
    void read_output();

    virtual ~TwiAccessRead();
};

}
}
}
}
