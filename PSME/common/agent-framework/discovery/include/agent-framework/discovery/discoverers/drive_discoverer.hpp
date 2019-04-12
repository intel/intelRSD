/*!
 * @brief Drive discoverer interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file drive_discoverer.hpp
 */

#pragma once


#include "agent-framework/module/model/drive.hpp"

namespace agent_framework {
namespace discovery {

class DriveDiscoverer {
public:

    /*! Virtual destructor */
    virtual ~DriveDiscoverer() { }

    /*!
     * @brief Perform discovery of drive objects.
     * @param parent_uuid Parent UUID.
     * @return Empty drive collection as implementation can be various depending on storage system.
     * */
    virtual std::vector<agent_framework::model::Drive> discover(const Uuid& parent_uuid);

};

}
}