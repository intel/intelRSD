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
 * @file partition/file_drive_interface.hpp
 */

#pragma once

#include "partition/abstract_drive_interface.hpp"

#include <string>

namespace partition {

/*!
 * @brief Drive interface based on simple file access
 */
class FileDriveInterface final : public AbstractDriveInterface {
public:

    FileDriveInterface() = default;

    FileDriveInterface(const FileDriveInterface&) = default;
    FileDriveInterface(FileDriveInterface&&) = default;

    FileDriveInterface& operator=(const FileDriveInterface&) = default;
    FileDriveInterface& operator=(FileDriveInterface&&) = default;

    virtual ~FileDriveInterface();

    /*! Implementations of pure virtual functions */
    virtual utils::ByteBuffer read(const std::string& target, uint64_t location, std::size_t num_of_bytes) const override;
    virtual void write(const std::string& target, uint64_t location, const utils::ByteBuffer& data) const override;
    virtual void write(const std::string& target, uint64_t location, uint64_t size, DataGenerator generator) const override;

};

}
