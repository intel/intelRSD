/*!
 * @brief Switch Agent class implementation.
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
 * @file agent/network/src/hal/switch_agent.cpp
 */

#include "hal/switch_agent.hpp"
#include "hal/eos_sdk/eos_agent.hpp"
#include "hal/eos_eapi/eapi_client.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/exceptions/exception.hpp"

using namespace agent::network::hal;

SwitchAgent::~SwitchAgent() {
    stop();
}

void SwitchAgent::start() {
    if (!m_started) {
        log_debug("network-agent", "Switch Agent is starting...");
        EosAgent::get_instance()->start();
        EapiClient::get_instance()->start();
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
        EapiClient::get_instance()->stop();
        m_started = false;
    }
}

std::vector<std::string> SwitchAgent::get_port_list() const {
    if (m_started) {
        return EosAgent::get_instance()->get_port_list();
    }
    std::vector<std::string> ret{};
    log_error("network-agent", "Switch Agent is not started");
    return ret;
}

void SwitchAgent::discover_port_neighbor_mac_addr() const {
    if (m_started) {
        EosAgent::get_instance()->discover_port_neighbor_mac_addr();
    }
    else {
        THROW(agent_framework::exceptions::NetworkError, "network-agent", "Switch agent is not running.");
    }
}

json::Json SwitchAgent::get_running_config() const {
    if (m_started) {
        return EapiClient::get_instance()->get_running_config();
    }
    THROW(agent_framework::exceptions::NetworkError, "switch-agent", "Switch agent is not running.");
}

bool SwitchAgent::is_switch_pfc_enabled() const {
    if (m_started) {
        return EapiClient::get_instance()->is_switch_pfc_enabled();
    }
    THROW(agent_framework::exceptions::NetworkError, "switch-agent", "Switch agent is not running.");
}
