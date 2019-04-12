/*!
 * @brief Network device builder class interface.
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
 * @file network_device_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/network_device.hpp"

namespace agent_framework {
namespace model {
class NetworkDeviceFunction;
}
}


namespace agent {
namespace compute {
namespace discovery {

class NetworkDeviceBuilder {
public:
    /*!
     * @brief Build default network device object.
     * @param parent_uuid Parent UUID.
     * @return Default build network device object.
     */
    static agent_framework::model::NetworkDevice build_default(const std::string& parent_uuid);

    /*!
     * @brief Updates the status of a network device object based on its network device function's status.
     * @param device reference to the updated network device.
     * @param ndf reference to the device's function.
     */
    static void update_from_function(agent_framework::model::NetworkDevice& device,
                                     const agent_framework::model::NetworkDeviceFunction& ndf);

};

}
}
}
