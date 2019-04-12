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
 * @file base_drive_handler_factory.hpp
 */

#pragma once



#include "base_drive_handler.hpp"

#include <memory>
#include <unordered_map>

namespace agent {
namespace storage {

/*!
 * Interface for drive handler factories.
 * Their responsibility is to provide proper drive handlers depending on the drive capabilities.
 */
class BaseDriveHandlerFactory {
public:
    using SPtr = std::shared_ptr<BaseDriveHandlerFactory>;

    /*!
     * @brief As drive handlers are cached, using this enum specifies to
     * reading directly from hardware or trying to read from local cache.
     */
    enum class HandlerMode {
        TRY_FROM_CACHE,
        READ_FROM_HW
    };

    /*!
     * @brief Virtual destructor.
     */
    virtual ~BaseDriveHandlerFactory();

    /*!
     * @brief Returns drive handler for a specified drive
     * @param name Name of the drive a handler is requested for
     * @param mode Indicated that data are get from local cache or from physical drive
     * @return Drive handler shared pointer
     */
    virtual BaseDriveHandler::SPtr get_handler(const std::string& name,
                                               HandlerMode mode = HandlerMode::READ_FROM_HW) = 0;


protected:
    std::unordered_map<std::string, BaseDriveHandler::SPtr> m_cache{};

};

}
}
