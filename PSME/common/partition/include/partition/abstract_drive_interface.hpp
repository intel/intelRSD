/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file partition/abstract_drive_interface.hpp
 */

#pragma once

#include "utils/byte_buffer.hpp"

#include <functional>

namespace partition {

using DataGenerator = std::function<const utils::ByteBuffer&()>;

/*!
 * @brief Base class for all drive access interfaces, used to read raw drive data
 */
class AbstractDriveInterface {
public:

    virtual ~AbstractDriveInterface();

    /*!
     * @brief Reads data from the drive
     * @param target Name of the target device
     * @param location Starting read offset
     * @param num_of_bytes Number of bytes to read
     * @return Read data
     */
    virtual utils::ByteBuffer read(const std::string& target, uint64_t location, std::size_t num_of_bytes) const = 0;

    /*!
     * @brief Writes data to the drive
     * @param target Name of the target device
     * @param location Starting write offset
     * @param data Data to be written
     */
    virtual void write(const std::string& target, uint64_t location, const utils::ByteBuffer& data) const = 0;

    /*!
     * @brief Writes bytes from the buffer obtained by repeatedly calling the generator function
     * @param target Name of the target device
     * @param location Starting write offset
     * @param size Number of bytes to be written
     * @param generator Generator returning new data buffer on each call
     */
    virtual void write(const std::string& target, uint64_t location, uint64_t size, DataGenerator generator) const = 0;

};

}
