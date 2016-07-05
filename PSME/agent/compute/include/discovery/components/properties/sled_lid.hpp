/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file discovery/components/properties/sled_lid.hpp
 *
 * @brief Sled Lid discovery interface
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_COMPONENTS_PROPERTIES_SLED_LID_HPP
#define	AGENT_COMPUTE_DISCOVERY_COMPONENTS_PROPERTIES_SLED_LID_HPP

#include <string>

namespace ipmi {
    class ManagementController;
}

namespace agent {
namespace compute {

namespace discovery {

/*!
 * @brief Discovers Sled Lid
 */
class DiscoverySledLid {
public:

    /*! IPMI Sensor Net Function*/
    static constexpr const uint8_t NET_FN_SENSOR = 0x7A;
    /*!
     * @brief Discovery Sled Lid
     *
     * @param mc IPMI management controller
     * @param module Compute module uuid
     */
    void discovery(ipmi::ManagementController& mc,
                   const std::string& module) const;
};

}
}
}

#endif	/* AGENT_COMPUTE_DISCOVERY_COMPONENTS_PROPERTIES_SLED_LID_HPP */

