/*!
 * @brief Zone discoverer interface.
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
 * @file zone_discoverer.hpp
 */

#pragma once


#include "agent-framework/module/model/zone.hpp"


namespace agent_framework {
namespace discovery {

class ZoneDiscoverer {
public:

    /*! Virtual destructor */
    virtual ~ZoneDiscoverer() { }

    /*!
     * @brief Perform discovery of zone objects.
     * @param parent_uuid Parent UUID.
     * @return Zone collection filled by the method with discovered data.
     * */
    virtual std::vector<agent_framework::model::Zone> discover(const Uuid& parent_uuid);

};

}
}