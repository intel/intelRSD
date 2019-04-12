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
 * @file local_drive_handler_factory.hpp
 */

#pragma once



#include "drive_handling/base_drive_handler_factory.hpp"

#include "sysfs/abstract_sysfs_interface.hpp"

#include <mutex>
#include <unordered_map>

namespace agent {
namespace storage {

/*!
 * Default implementation of the BaseDriveHandlerFactory for block devices.
 */
class LocalDriveHandlerFactory : public BaseDriveHandlerFactory {
public:

    /*!
     * @brief Constructs a valid drive handler factory
     * @param sysfs_interface Interface for sysfs to be used by the factory and drive handlers.
     */
    LocalDriveHandlerFactory(std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface)
        : m_sysfs_interface(sysfs_interface) {}

    /*!
     * @brief virtual Destructor
     */
    virtual ~LocalDriveHandlerFactory();


    virtual BaseDriveHandler::SPtr get_handler(const std::string& name, HandlerMode mode) override;


private:
    std::shared_ptr<sysfs::AbstractSysfsInterface> m_sysfs_interface{};

};

}
}
