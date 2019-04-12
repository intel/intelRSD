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
 * @file drive_handler_factory.hpp
 */

#pragma once

#include "tools/drive_discovery/base_drive_handler_factory.hpp"
#include "nvme/abstract_nvme_interface.hpp"
#include "partition/abstract_drive_interface.hpp"
#include "nvme/utils.hpp"

#include <mutex>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * Default implementation of the BaseDriveHandlerFactory.
 */
class DriveHandlerFactory : public BaseDriveHandlerFactory {
public:

    /*!
     * @brief Constructs a valid drive handler factory
     * @param nvme_interface Nvme interface to be used by the factory and drive handlers
     * @param nvme_interface Drive interface to be used by the factory and drive handlers
     */
    DriveHandlerFactory(std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface,
                        std::shared_ptr<::partition::AbstractDriveInterface> drive_interface)
        : m_nvme_interface(nvme_interface), m_drive_interface(drive_interface) {}

    /*!
     * @brief virtual Destructor
     */
    virtual ~DriveHandlerFactory();

    /*!
     * @brief Returns drive handler for a specified drive
     * @param name Name of the drive a handler is requested for
     * @return Drive handler shared pointer
     */
    virtual std::shared_ptr<BaseDriveHandler> get_handler(const std::string& name) const override;

private:

    mutable std::recursive_mutex m_mutex{};

    std::shared_ptr<::nvme::AbstractNvmeInterface> m_nvme_interface{};
    std::shared_ptr<::partition::AbstractDriveInterface> m_drive_interface{};

};

}
}
}
