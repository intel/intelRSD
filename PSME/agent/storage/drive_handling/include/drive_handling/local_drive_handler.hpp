/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file local_drive_handler.hpp
 */

#pragma once



#include "drive_handling/base_drive_handler.hpp"
#include "ioctl/ioctl_interface.hpp"

#include "sysfs/abstract_sysfs_interface.hpp"

#include <mutex>

namespace agent {
namespace storage {

/*!
 * Drive handler implementation for local block devices
 */
class LocalDriveHandler : public BaseDriveHandler {
public:

    /*!
     * @brief Constructs a valid handler.
     * @param name Name of the drive device.
     * @param sysfs_interface Pointer to sysfs interface.
     * @param ioctl_interface Pointer to ioctl interface.
     */
    LocalDriveHandler(const std::string& name,
                      std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface,
                      std::shared_ptr<ioctl::IoctlInterface> ioctl_interface)
        : BaseDriveHandler(name), m_sysfs_interface(sysfs_interface), m_ioctl_interface(ioctl_interface) { }

    /*!
     * @brief Virtual destructor.
     */
    virtual ~LocalDriveHandler();

    /* Implemented pure virtual methods from the base class */
    virtual void load() override;
    virtual DriveData get_drive_data() const override;
    virtual SmartData get_smart_info() const override;


private:

    std::shared_ptr<sysfs::AbstractSysfsInterface> m_sysfs_interface{};
    std::shared_ptr<ioctl::IoctlInterface> m_ioctl_interface{};

    std::string m_capacity_value{};
    ioctl::IoctlInterface::HdioDataPtr m_ata_attributes{};
};

}
}
