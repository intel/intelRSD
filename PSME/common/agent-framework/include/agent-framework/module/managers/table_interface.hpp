/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 */
#pragma once
#include "agent-framework/module/enum/common.hpp"
#include <string>

namespace agent_framework {
namespace module {

/*!
 * @brief Base class for GenericManager
 */
class TableInterface {
public:
    enum class UpdateStatus {
        NoUpdate,
        StatusChanged,
        Updated,
        Added
    };

    /*!
     * @brief Destructor
     */
    virtual ~TableInterface();

    /*!
     * @brief Returns component of resource stored in the table
     * @return component type
     */
    virtual model::enums::Component get_component() = 0;

    /*!
     * @brief Checks if given entry exists in the table
     * @return true if entry with given uuid exists, false otherwise
     */
    virtual bool entry_exists(const std::string& uuid) = 0;
};


}
}
