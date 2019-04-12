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
 * @file unbind_port.hpp
 * @brief Unbind Port Command
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

/*! Unbind Port Command */
class UnbindPort final : public Command {
public:
    /*! Port Partition P2P Unbind Command input memory size */
    static constexpr std::uint8_t PORT_PARTITION_P2P_UNBIND_INPUT_MEMORY_SIZE = 4;
    /*! SubCommand offset */
    static constexpr std::uint8_t OFFSET_SUB_COMMAND = 0;
    /*! Partition ID offset */
    static constexpr std::uint8_t OFFSET_PARTITION_ID = 1;
    /*! Logical Bridge ID offset */
    static constexpr std::uint8_t OFFSET_LOGICAL_BRIDGE_ID = 2;
    /*! Physical Port ID offset */
    static constexpr std::uint8_t OFFSET_OPTION = 3;
    /*! UnbindPort subcommand code */
    PortPartitionP2PBindSubCommandCode sub_command = PortPartitionP2PBindSubCommandCode::UNBIND_PORT;

    /*! Default constructor */
    UnbindPort(AccessInterface* iface) : Command(iface, CommandCode::PORT_PARTITION_P2P_BINDING) {}

#pragma pack(push,1)
    /*! Unbind Port Command input structure */
    union {
        struct {
            std::uint8_t partition_id;
            std::uint8_t logical_bridge_id;
            UnbindPortOption option;
        } fields;
        std::uint8_t raw[PORT_PARTITION_P2P_UNBIND_INPUT_MEMORY_SIZE];
    } input{};

    /*! Unbind Port Command output structure */
    union {
        struct {
            PortPartitionP2PBindingReturnValue ret_value;
        } fields;
    } output{};
#pragma pack(pop)

    /*! Write input data */
    void write_input();

    /*! Read output data */
    void read_output();

    virtual ~UnbindPort();
};

}
}
}
}
