/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
 * @file switch_agent.cpp
 *
 * @brief switch agent
 * */

#include "hal/switch_agent.hpp"
#include "hal/eos_sdk/eos_agent.hpp"
#include "logger/logger_factory.hpp"

using namespace std;
using namespace agent::network::hal;

SwitchAgent::~SwitchAgent() {
    stop();
}

void SwitchAgent::start() {
    if (!m_started) {
        log_debug(GET_LOGGER("network-agent"), "Switch Agent is starting...");
        EosAgent::get_instance()->start();
        m_started = true;
    }
}

void SwitchAgent::wait_until_initialized() {
    if (m_started) {
        EosAgent::get_instance()->wait_until_initialized();
    }
}

void SwitchAgent::stop() {
    if (m_started) {
        EosAgent::get_instance()->stop();
        m_started = false;
    }
}

vector<string> SwitchAgent::get_port_list() const {
    if (m_started) {
        return EosAgent::get_instance()->get_port_list();
    }
    vector<string> ret{};
    log_error(GET_LOGGER("network-agent"), "Switch Agent is not started");
    return ret;
}

void SwitchAgent::discover_port_neighbor_mac_addr() const {
    if (m_started) {
        EosAgent::get_instance()->discover_port_neighbor_mac_addr();
    }
    else {
        log_error(GET_LOGGER("network-agent"), "Switch Agent is not started");
    }
}
