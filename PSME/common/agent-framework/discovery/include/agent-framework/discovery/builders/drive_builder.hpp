/*!
 * @brief Drive builder class interface.
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
 * @file drive_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/drive.hpp"


namespace agent_framework {
namespace discovery {

class DriveBuilder {
public:

    /*!
     * @brief Build default drive object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default drive object.
     */
    static model::Drive build_default(const Uuid& parent_uuid);

    /*!
     * @brief Update drive object to critical state.
     * @param drive Drive to be updated.
     */
    static void update_critical_state(model::Drive& drive);

};

}
}
