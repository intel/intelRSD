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
 * @file global_address_space_registers.cpp
 * @brief Global Address Space Registers implementation
 * */

#include "gas/global_address_space_registers.hpp"

#include <chrono>
#include <thread>



using namespace agent::pnc::gas;


GlobalAddressSpaceRegisters::~GlobalAddressSpaceRegisters() {}


constexpr std::uint16_t GlobalAddressSpaceRegisters::MRPC_DELAY_MS;

std::mutex GlobalAddressSpaceRegisters::m_mrpc_mutex{};
std::mutex GlobalAddressSpaceRegisters::m_top_mutex{};
std::mutex GlobalAddressSpaceRegisters::m_partition_mutex{};
std::mutex GlobalAddressSpaceRegisters::m_csr_mutex{};


mrpc::CommandStatus GlobalAddressSpaceRegisters::execute_cmd(mrpc::Command& cmd) const {
    std::lock_guard<std::mutex> lock(m_mrpc_mutex);

    mrpc::CommandStatus status{mrpc::CommandStatus::IN_PROGRESS};

    cmd.write_input();
    cmd.run();

    // Waits until operation is IN_PROGRESS or IDLE (device is busy/waiting).
    std::this_thread::sleep_for(std::chrono::milliseconds(GlobalAddressSpaceRegisters::MRPC_DELAY_MS));
    while (mrpc::CommandStatus::IN_PROGRESS == status) {
        std::this_thread::sleep_for(std::chrono::milliseconds(GlobalAddressSpaceRegisters::MRPC_DELAY_MS));
        status = cmd.read_status();
    }

    // If error occurs we try to read MRPC status 3 times and extend delay.
    auto error_delay_ms = 2 * GlobalAddressSpaceRegisters::MRPC_DELAY_MS; // 20ms.
    unsigned tries = 3;
    while (mrpc::CommandStatus::FAILED == status && tries > 0) {
        log_warning("gas-tool", "MRPC status: FAILED. Will try again...");
        std::this_thread::sleep_for(std::chrono::milliseconds(error_delay_ms));
        status = cmd.read_status();
        tries--;
        error_delay_ms += 50;
    }

    // If still error we return status without command status read.
    if (mrpc::CommandStatus::FAILED == status) {
        log_error("gas-tool", "MRPC FAILED after 3 tries.");
        return status;
    }

    // MRPC is DONE. We read command status.
    cmd.read_output();

    std::this_thread::sleep_for(std::chrono::milliseconds(GlobalAddressSpaceRegisters::MRPC_DELAY_MS));

    return status;
}


template<typename T>
mrpc::CommandStatus GlobalAddressSpaceRegisters::read_register(T& reg) const {
    if (nullptr == m_iface) {
        return CommandStatus::FAILED;
    }
    try {
        reg.read(m_iface);
        return CommandStatus::DONE;
    }
    catch (const std::exception&) {
        return CommandStatus::FAILED;
    }
}


template mrpc::CommandStatus GlobalAddressSpaceRegisters::read_register(top::TopLevelRegisters&) const;


template mrpc::CommandStatus GlobalAddressSpaceRegisters::read_register(csr::ConfigurationSpaceRegister&) const;


template mrpc::CommandStatus GlobalAddressSpaceRegisters::read_register(partition::PartitionConfiguration&) const;


void GlobalAddressSpaceRegisters::read_top() {
    std::lock_guard<std::mutex> lock(m_top_mutex);
    if (CommandStatus::DONE != read_register(top)) {
        throw std::runtime_error("GAS error: cannot read TOP data.");
    }
}


void GlobalAddressSpaceRegisters::read_partition(std::uint8_t partition_id) {
    std::lock_guard<std::mutex> lock(m_partition_mutex);
    partition.input.fields.partition_id = partition_id;
    if (CommandStatus::DONE != read_register(partition)) {
        throw std::runtime_error(
            std::string{"GAS error: cannot read Partition Configuration data, partitionId = "}
            + std::to_string(unsigned{partition_id}));
    }
}


void GlobalAddressSpaceRegisters::read_csr(std::uint8_t instance_id) {
    std::lock_guard<std::mutex> lock(m_csr_mutex);
    csr.input.fields.instance_id = instance_id;
    if (CommandStatus::DONE != read_register(csr)) {
        throw std::runtime_error(
            std::string{"GAS error: cannot read CSR data, instanceId = "}
            + std::to_string(unsigned{instance_id}));
    }
}


GlobalAddressSpaceRegisters GlobalAddressSpaceRegisters::get_default(const std::string& path) {
    GlobalAddressSpaceRegisters gas{};
    try {
        AccessInterfaceFactory aif{};
        AccessInterface* iface = aif.get_interface();
        iface->init(path);
        gas.init(iface);
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string{"Cannot initialize PCIe interface: "} + e.what());
    }
    return gas;
}
