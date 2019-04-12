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
 * @file top_level_registers.cpp
 * @brief Top Level Registers
 * */

#include "logger/logger_factory.hpp"
#include "gas/top/top_level_registers.hpp"

using namespace agent::pnc::gas::top;

TopLevelRegisters::~TopLevelRegisters(){}

void TopLevelRegisters::read(AccessInterface* iface) {
    iface->read(&output.fields.stk_bif_valid, TOP_SETTING_REG_SIZE, TOP_SETTING_REG_OFFSET);

    log_debug("pnc-top", "Partitions number: " << std::uint32_t(output.fields.part_num));
    log_debug("pnc-top", "Current Partition ID: " << std::uint32_t(output.fields.current_partition_id));
    log_debug("pnc-top", "PCIe Functions number: " << std::uint32_t(output.fields.pci_fn_num));

    //for(size_t fn = 0; fn < output.fields.pci_fn_num; fn++) {
    //    log_debug("pnc-top", "PCIe Functions " << fn << " is assigned to Physical Port "
    //            << std::uint32_t(output.fields.pcifn_switch_ports[fn]));
    //}
}
