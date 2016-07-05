/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file hw/fm10000/network_controller_manager.cpp
 *
 * @brief Implementation of network controller manager
 * */


#include "network_config.hpp"

#include "hw/fm10000/network_controller_manager.hpp"

using namespace agent::network::hw::fm10000;

static NetworkController *g_network_controller = nullptr;

NetworkController* NetworkControllerManager::get_network_controller() {
    if (nullptr == g_network_controller) {
        g_network_controller = new NetworkController();
    }
    return g_network_controller;
}

void NetworkControllerManager::cleanup(){
    if (nullptr == g_network_controller) {
        return;
    }

    delete g_network_controller;
    g_network_controller = nullptr;
}

NetworkControllerManager::~NetworkControllerManager() {}
