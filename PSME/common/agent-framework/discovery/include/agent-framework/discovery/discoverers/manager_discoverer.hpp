/*!
 * @brief Basic Manager discoverer interface.
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
 * @file manager_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/model/manager.hpp"



namespace agent_framework {
namespace discovery {

class ManagerDiscoverer {
public:

    /*! Virtual destructor */
    virtual ~ManagerDiscoverer() { }

    /*!
     * @brief Perform discovery of manager object.
     * @return Manager object filled with discovered data.
     * */
    virtual model::Manager discover();
};

}
}