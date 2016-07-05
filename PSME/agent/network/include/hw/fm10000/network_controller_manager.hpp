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
 * @file hw/fm10000/network_controller_manager.hpp
 *
 * @brief Implementation of network interface
 * */

#ifndef AGENT_NETWORK_HW_FM10000_NETWORK_CONTROLLER_MANAGER_HPP
#define AGENT_NETWORK_HW_FM10000_NETWORK_CONTROLLER_MANAGER_HPP

#include "network_controller.hpp"

namespace agent {
namespace network {
namespace hw {
namespace fm10000 {

/*! Network Controller Manager */
class NetworkControllerManager {
public:
    /*!
     * @brief Returns global pointer to Network Controller.
     * @return Pointer to Network Controller
     * */
    static NetworkController* get_network_controller();

    /*!
     * @brief Network Controller cleanup
     * */
    static void cleanup();

    ~NetworkControllerManager();
private:
    NetworkControllerManager(){}

    NetworkControllerManager(const NetworkControllerManager&) = delete;
    NetworkControllerManager& operator=(const NetworkControllerManager&) = delete;
};

}
}
}
}

#endif /* AGENT_NETWORK_HW_FM10000_NETWORK_CONTROLLER_MANAGER_HPP */
