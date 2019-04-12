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
 * @file top_level_registers.hpp
 * @brief Top Level Registers
 * */

#pragma once

#include "gas/access_interface.hpp"
#include "gas/utils.hpp"

#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

class GlobalAddressSpaceRegisters;

/*! TOP namespace */
namespace top {

/*! Top Level Registers class */
class TopLevelRegisters final {
private:
    friend class agent::pnc::gas::GlobalAddressSpaceRegisters;

    /* Reads TopLevelRegisters memory region. */
    void read(AccessInterface* iface);

public:

    /*! Default constructor */
    TopLevelRegisters() {}

    /*! Copy constructor */
    TopLevelRegisters(const TopLevelRegisters&) = default;

    /*! Assignment operator */
    TopLevelRegisters& operator=(const TopLevelRegisters&) = default;

#pragma pack(push, 1)
    /*! Top Level Registers output structure  */
    union {
        struct {
            std::uint8_t stk_bif_valid;
            std::uint8_t stk0_port_valid;
            std::uint8_t stk1_port_valid;
            std::uint8_t stk2_port_valid;
            std::uint8_t stk3_port_valid;
            std::uint8_t stk4_port_valid;
            std::uint8_t stk5_port_valid;
            std::uint8_t part_num;
            std::uint8_t current_partition_id;
            std::uint8_t pci_fn_num;
            std::uint8_t pcifn_switch_ports[TOP_SETTING_REG_SIZE - 10];
        } fields{};
        std::uint8_t raw[TOP_SETTING_REG_SIZE];
    } output{};
#pragma pack(pop)

    ~TopLevelRegisters();
};

}
}
}
}
