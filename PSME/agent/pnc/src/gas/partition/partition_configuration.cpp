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
 * @file partition_configuration.cpp
 * @brief Partition Configuration
 * */

#include "logger/logger_factory.hpp"
#include "gas/partition/partition_configuration.hpp"
#include "safe-string/safe_lib.hpp"

using namespace agent::pnc::gas::partition;

PartitionConfiguration::~PartitionConfiguration(){}

void PartitionConfiguration::read(AccessInterface* iface) {

    std::uint8_t data[PARTITION_USED_REG_SIZE];

    /*
     * Partition Configuration memory map:
     * 0x0000: |      Status        |
     * 0x0004: |      State         |
     * 0x0008: |     Port_Cnt       |
     * 0x000c: |  USP Port Mode     |
     * 0x0010: | USP Instance ID    |
     * 0x0014: | VEP Instance ID    |
     * 0x0018: | DSP 00 Instance ID |
     * 0x001C: | DSP 01 Instance ID |
     * ...
     */

    iface->read(data, PARTITION_USED_REG_SIZE, PARTITION_REG_OFFSET +
            (PARTITION_REG_SIZE/PM85X6_MAX_PARTITIONS_NUMBER) * input.fields.partition_id);

    memcpy_s(&output.fields, sizeof(output.fields), data, sizeof(output.fields));

    log_debug("pnc-mrpc", "Partition: " << std::uint32_t(input.fields.partition_id)
                                   << " Status: " << std::uint32_t(output.fields.status)
                                   << " State: " << std::uint32_t(output.fields.state)
                                   << " Ports Number:" << std::uint32_t(output.fields.ports_number));
}
