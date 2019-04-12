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
 * @file partition_configuration.hpp
 * @brief Partition Configuration
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

/*! Partition Configuration namespace */
namespace partition {

/*! Class representing Partition Configuration registers from GAS */
class PartitionConfiguration final {
private:

    static constexpr std::uint8_t PARTITION_STATUS_OFFSET = 0x0;
    static constexpr std::uint8_t PARTITION_STATE_OFFSET = 0x4;
    static constexpr std::uint8_t PORT_COUNT_OFFSET = 0x8;

    friend class agent::pnc::gas::GlobalAddressSpaceRegisters;

    /* Reads TopLevelRegisters memory region. */
    void read(AccessInterface* iface);

public:

    /*! Value representing Invalid Port Instance ID */
    static constexpr std::uint32_t INVALID_INST_ID = 0xFFFFFFFF;

    /*! Default constructor */
    PartitionConfiguration() {}

    /*! Copy constructor */
    PartitionConfiguration(const PartitionConfiguration&) = default;

    /*! Assignment operator */
    PartitionConfiguration& operator=(const PartitionConfiguration&) = default;

#pragma pack(push, 1)
    /*! Partition Configuration input structure */
    union {
        struct {
            std::uint8_t partition_id;
        } fields;
    } input{};

    /*! Partition Configuration output structure */
    union {
        struct {
            PartitionStatus status;
            PartitionState state;
            std::uint32_t ports_number;
            UspPortMode usp_port_mode;
            std::uint32_t usp_inst_id;
            std::uint32_t vep_inst_id;
            std::uint32_t dsp_inst_id[PARTITION_MAX_DSP_PORTS_NUM];
        } fields;
        std::uint8_t raw[PARTITION_REG_SIZE];
    } output{};
#pragma pack(pop)
    ~PartitionConfiguration();
};

}
}
}
}
