/*!
 * @brief Chassis builder class interface.
 *
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
 * @file chassis_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/chassis.hpp"


namespace agent_framework {
namespace discovery {


class ChassisBuilder {
public:

    /*!
     * @brief Build default chassis object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default chassis object.
     */
    static agent_framework::model::Chassis build_default(const Uuid& parent_uuid);
};

}
}
