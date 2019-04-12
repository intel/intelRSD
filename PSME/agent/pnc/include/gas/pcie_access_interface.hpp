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
 * @file pcie_access_interface.hpp
 * @brief PCIe GAS access interface
 * */

#pragma once

#include "gas/access_interface.hpp"

#include <mutex>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {


/*! PCIe GAS access interface */
class PcieAccessInterface final : public AccessInterface {
    static PcieAccessInterface *iface;
    static bool is_initialized;
    static int memory_file;
    std::string memory_file_path{};

    std::mutex iface_mutex{};

    std::uint64_t get_file_size();
    void* write_base(uint32_t size, uint32_t offset, off_t pa_offset);
    void* read_base(uint32_t size, uint32_t offset, off_t pa_offset);

    /*! Default constructor */
    PcieAccessInterface() : AccessInterface() {}

public:

    /*!
     * @brief PcieAccessInterface singleton method.
     * @return PcieAccessInterface pointer
     */
    static PcieAccessInterface* get_instance();

    /*!
     * @brief Opens and maps file to memory
     * @param path Path to file to be mapped.
     */
    void init(const std::string& path);

    /*!
     * @brief Unmaps and closes memory mapped file.
     */
    void deinit();

    /*!
     * @brief Method used to write memory region via PCIe.
     * @param data Data to write.
     * @param size Data size to write.
     * @param offset Region offset to write.
     */
    void write(std::uint8_t* data, std::uint32_t size, std::uint32_t offset);

    /*!
     * @brief Method used to read memory region via PCIe.
     * @param data Buffer to store read data.
     * @param size Data size to read.
     * @param offset Region offset to read.
     */
    void read(std::uint8_t* data, std::uint32_t size, std::uint32_t offset);

    ~PcieAccessInterface();
};

}
}
}
