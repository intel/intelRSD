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
 * @file link_status_retrieve.hpp
 * @brief Link Status Retrieve Command
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

/*! Link Status Retrieve Command */
class LinkStatusRetrieve final : public Command {
public:
    /*! Link Status Retrieve Command input memory size */
    static constexpr std::uint8_t LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE = 8;
    /*! Link Status Retrieve Command output memory size */
    static constexpr std::uint8_t LINK_STATUS_RETRIEVE_OUTPUT_MEMORY_SIZE = 10;
    /*! SubCommand offset */
    static constexpr std::uint8_t OFFSET_INPUT_DATA = 0;

    /*! Default constructor */
    LinkStatusRetrieve(AccessInterface* iface) : Command(iface, CommandCode::LINK_STATUS_RETRIEVE) {}
    /*! Copy constructor */
    LinkStatusRetrieve(const LinkStatusRetrieve&) = default;
    /*! Copy assignment operator */
    LinkStatusRetrieve& operator=(const LinkStatusRetrieve&) = default;

#pragma pack(push, 1)
    /*! Link Status Retrieve Command input structure */
    union {
        struct {
            std::uint64_t input_data;
        } fields;
        std::uint8_t raw[LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE];
    } input{};

    union {
        struct {
            LinkStatusRetrieveReturnValue ret_value;
            uint8_t port_id;
            uint8_t partition_id;
            uint8_t logical_bridge_id;
            uint8_t stack_id_port_id;
            uint8_t cfg_link_width;
            uint8_t neg_link_width;
            uint8_t ups_flag;
            uint8_t linkup_link_rate;
            uint8_t major;
            uint8_t minor;
        } fields;
    } output{};
#pragma pack(pop)

    /*! Write input data */
    void write_input();

    /*! Read output data */
    void read_output();

    virtual ~LinkStatusRetrieve();
};
}
}
}
}
