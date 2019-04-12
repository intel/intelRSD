/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file drive_reader.hpp
 */

#pragma once

#include "tools/drive_discovery/base_drive_reader.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"

#include <memory>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * DriveReader implemented based on the sysfs
 */
class DriveReader : public BaseDriveReader {
public:

    /*!
     * @brief constructs a valid drive reader object
     * @param sysfs_interface Sysfs Interface implementation to be used in the reader
     */
    DriveReader(std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface)
        : m_sysfs_interface(sysfs_interface) {}

    /*!
     * @brief virtual Destructor
     */
    virtual ~DriveReader();

    /*!
     * @brief Returns list of all drives found in the system
     * @return List of ids of the found drives
     */
    virtual std::vector<std::string> get_drives() const override;

private:

    std::shared_ptr<sysfs::AbstractSysfsInterface> m_sysfs_interface{};

};

}
}
}
