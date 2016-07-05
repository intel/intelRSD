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
 * */

#include "network_config.hpp"
#include "agent-framework/command/network/initialization.hpp"
#include "agent-framework/logger_ext.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "hw/fm10000/network_controller_manager.hpp"

using namespace agent_framework::command;
using namespace agent::network::hw;

/*! Initialization command */
namespace agent {
namespace network {
namespace hw {
namespace fm10000 {

using namespace agent_framework;

class Initialization : public command::network::Initialization {
public:
    /*! Initialization */
    Initialization();

    /*! Deinitialization */
    ~Initialization();
};
}
}
}
}

fm10000::Initialization::Initialization() {
    log_debug(GET_LOGGER("fm10000"), "Initialization");
    auto network_controller = NetworkControllerManager::get_network_controller();
    network_controller->initialize();
}

fm10000::Initialization::~Initialization() {
    log_debug(GET_LOGGER("fm10000"), "Deinitialization");
    auto network_controller = NetworkControllerManager::get_network_controller();
    network_controller->deinitialize();
    NetworkControllerManager::cleanup();
}

static Command::Register<fm10000::Initialization> g("fm10000");
