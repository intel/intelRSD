/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file responses/psme/component_notification.hpp
 * @brief Definition of PSME component_notification response
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace responses {

/*!
 * ComponentNotification response
 * Currently each command is registered both as a notification and as a method
 * therefore it is needed to add an empty response that can be (de)serialized (from)to json
 */
class ComponentNotification {
public:

    /*!
     * @brief Transform response to Json
     * @return created Json value
     */
    json::Json to_json() const {
        return json::Json{};
    }

    /*!
     * @brief create ComponentNotification from Json
     * @return new ComponentNotification
     */
    static ComponentNotification from_json(const json::Json&) {
        return ComponentNotification{};
    }
};

}
}
}
