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
 * @file nvme_drive_handler_factory.hpp
 */

#pragma once



#include "drive_handling/base_drive_handler_factory.hpp"

#include "nvme/abstract_nvme_interface.hpp"

#include <mutex>

namespace agent {
namespace storage {

/*!
 * Default NVMe implementation of the BaseDriveHandlerFactory.
 */
class NvmeDriveHandlerFactory : public BaseDriveHandlerFactory {
public:

    /*!
     * @brief Constructs a valid drive handler factory
     * @param nvme_interface NVMe interface to be used by the factory and drive handlers
     */
    NvmeDriveHandlerFactory(std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface)
        : m_nvme_interface(nvme_interface) {}

    /*!
     * @brief virtual Destructor
     */
    virtual ~NvmeDriveHandlerFactory();


    virtual BaseDriveHandler::SPtr get_handler(const std::string& name, HandlerMode mode) override;

private:

    mutable std::recursive_mutex m_mutex{};

    std::shared_ptr<::nvme::AbstractNvmeInterface> m_nvme_interface{};
};

}
}
