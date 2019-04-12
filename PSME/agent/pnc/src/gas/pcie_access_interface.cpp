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
 * @file pcie_access_interface.cpp
 * @brief GAS PCIe access interface implementation
 * */

#include "gas/pcie_access_interface.hpp"
#include "logger/logger_factory.hpp"

#include "safe-string/safe_lib.hpp"

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>


using namespace agent::pnc::gas;

PcieAccessInterface* PcieAccessInterface::iface = nullptr;
bool PcieAccessInterface::is_initialized = false;
int PcieAccessInterface::memory_file{};

PcieAccessInterface::~PcieAccessInterface() {
    if (nullptr != iface) {
        delete iface;
    }
    iface = nullptr;
    deinit();
}

PcieAccessInterface* PcieAccessInterface::get_instance() {
    if (nullptr == iface) {
        iface = new PcieAccessInterface();
    }
    return iface;
}

uint64_t PcieAccessInterface::get_file_size() {
    struct stat st;

    if (stat(memory_file_path.c_str(), &st) == -1) {
        log_debug("agent", "File stat error: " << strerror(errno));
        throw std::runtime_error("Cannot stat device memory file.");
    }
    return uint64_t(st.st_size);
}

void PcieAccessInterface::init(const std::string& path) {

    if (is_initialized) {
        log_debug("agent", "Pcie Access Interface is already initialized.");
        return;
    }
    if (path.empty()) {
        throw std::runtime_error("Path to PCIe device memory file is incorrect");
    }
    memory_file_path = path;

    memory_file = open(memory_file_path.c_str(), O_RDWR, 0);
    if (!memory_file) {
        throw std::runtime_error("Cannot open PCIe Device memory file.");
    }

    is_initialized = true;
    log_debug("agent", "PCIe device memory file opened.");
}

void PcieAccessInterface::deinit() {
    if (is_initialized) {
        close(memory_file);
        memory_file = -1;
        is_initialized = false;
    }
}

void* PcieAccessInterface::write_base(std::uint32_t size, std::uint32_t offset, off_t pa_offset) {
    if (!is_initialized) {
        throw std::runtime_error("Pcie Access Interfaces is not initialized.");
    }

    auto length = get_file_size();
    if ((offset + size) > length) {
        throw std::runtime_error("Write area to large.");
    }

    auto mapped_memory =
            mmap(nullptr, size + offset - pa_offset, PROT_WRITE, MAP_SHARED , memory_file, pa_offset);
    if (MAP_FAILED == mapped_memory) {
        log_debug("agent", "File mapping error: " << strerror(errno));
        throw std::runtime_error("Cannot map PCIe Device memory file.");
    }

    return mapped_memory;
}

void* PcieAccessInterface::read_base(std::uint32_t size, std::uint32_t offset, off_t pa_offset) {
    if (!is_initialized) {
        throw std::runtime_error("Pcie Access Interfaces is not initialized.");
    }

    auto length = get_file_size();
    if ((offset + size) > length) {
        throw std::runtime_error("Read area to large.");
    }

    auto mapped_memory =
            mmap(nullptr, size + offset - pa_offset, PROT_READ, MAP_PRIVATE , memory_file, pa_offset);
    if (MAP_FAILED == mapped_memory) {
        log_debug("agent", "File mapping error: " << strerror(errno));
        throw std::runtime_error("Cannot map PCIe Device memory file.");
    }

    return mapped_memory;
}

void PcieAccessInterface::write(std::uint8_t* data, std::uint32_t size, std::uint32_t offset) {

    if (!data) {
        log_debug("agent", "Write buffer is broken.");
        throw std::runtime_error("Cannot write memory.");
    }

    std::lock_guard<std::mutex> lock(iface_mutex);

    off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
    uint8_t* mapped_memory = static_cast<uint8_t*>(write_base(size, offset, pa_offset));

    memcpy_s(mapped_memory + offset - pa_offset, size, data, size);

    if (-1 == munmap(mapped_memory, size + offset - pa_offset)) {
        log_debug("agent", "Memory unmapping error: " << strerror(errno));
        throw std::runtime_error("Cannot unmap PCIe Device memory file.");
    }
}

void PcieAccessInterface::read(std::uint8_t* data, std::uint32_t size, std::uint32_t offset) {

    if (!data) {
        log_debug("agent", "Raed buffer is broken.");
        throw std::runtime_error("Cannot read memory.");
    }

    std::lock_guard<std::mutex> lock(iface_mutex);

    off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
    uint8_t* mapped_memory = static_cast<uint8_t*>(read_base(size, offset, pa_offset));

    memcpy_s(data, size, mapped_memory + offset - pa_offset, size);

    if (-1 == munmap(mapped_memory, size + offset - pa_offset)) {
        log_debug("agent", "Memory unmapping error: " << strerror(errno));
        throw std::runtime_error("Cannot unmap PCIe Device memory file.");
    }
}
