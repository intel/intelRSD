/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 *
 * @file lvm_erase_task.hpp
 * @brief LVM erase task
 * */

#pragma once



#include <string>



namespace storage {
namespace lvm {

/*! @brief Class supports erasing of LVM devices */
class LvmEraseTask {
public:

    /*!
     * @brief Task for secure erase of a volume data.
     * @param path Path to the device.
     * @param size Capacity in bytes of the device to be cleared.
     * @param fast True if erase method should be fast, false to replace data with random noise.
     */
    explicit LvmEraseTask(const std::string& path, std::uint64_t size, bool fast);


    /*! @brief Starts erasing */
    void operator()();

private:
    std::string m_destination_path{};
    std::uint64_t m_size{};
    bool m_fast{false};
};

}
}
