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
 * @file port_binding_info.hpp
 * @brief Port Binding Info Command
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

/*! Port Binding Info Command */
class PortBindingInfo final : public Command {

#pragma pack(push, 1)
    struct port_binding_info_t {
        std::uint8_t phy_port_id;
        std::uint8_t partition_id;
        std::uint8_t logical_bridge_id;
        std::uint8_t state_operation_result;
    };
#pragma pack(pop)

public:
    /*! Port Binding Info Command input memory size */
    static constexpr std::uint8_t PORT_BINDING_INFO_INPUT_MEMORY_SIZE = 4;
    /*! Port Binding Info Command output memory size */
    static constexpr std::uint8_t PORT_BINDING_INFO_OUTPUT_MEMORY_SIZE = 196;
    /*! SubCommand offset */
    static constexpr std::uint8_t OFFSET_SUB_COMMAND = 0;
    /*! Physical Port ID offset */
    static constexpr std::uint8_t OFFSET_PHY_PORT_ID = 1;
    /*! Reserved 0 offset */
    static constexpr std::uint8_t OFFSET_RESERVED_0 = 2;
    /*! Reserved 1 offset */
    static constexpr std::uint8_t OFFSET_RESERVED_1 = 3;
    /*! Port binding entry size */
    static constexpr std::uint8_t PORT_BINDING_ENTRY_SIZE = 4;

    /*! BindPort subcommand code */
    PortPartitionP2PBindSubCommandCode sub_command = PortPartitionP2PBindSubCommandCode::PORT_BINDING_INFO;
    /*! Default constructor */
    PortBindingInfo(AccessInterface* iface) : Command(iface, CommandCode::PORT_PARTITION_P2P_BINDING) {}
    /*! Copy constructor */
    PortBindingInfo(const PortBindingInfo&) = default;
    /*! Copy assignment operator */
    PortBindingInfo& operator=(const PortBindingInfo&) = default;

#pragma pack(push,1)
    /*! Port Binding Info Command input structure */
    union {
        struct {
            std::uint8_t phy_port_id;
        } fields;
        std::uint8_t raw[PORT_BINDING_INFO_INPUT_MEMORY_SIZE];
    } input{};

    /*! Port Binding Info Command output structure */
    union {
        struct {
            PortPartitionP2PBindingReturnValue ret_value;
            std::uint8_t info_count;
            std::uint8_t reserved[3];
            port_binding_info_t port_binding_info[PM85X6_PHY_PORTS_NUMBER];
        } fields;
        std::uint8_t raw[PORT_BINDING_INFO_OUTPUT_MEMORY_SIZE];
    } output{};
#pragma pack(pop)

    /*! Write input data */
    void write_input();

    /*! Read output data */
    void read_output();

    virtual ~PortBindingInfo();
};
}
}
}
}
