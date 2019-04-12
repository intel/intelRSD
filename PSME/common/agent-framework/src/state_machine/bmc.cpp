/*!
 * @brief Implementation of Bmc class
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file bmc.cpp
 */

#include "agent-framework/state_machine/bmc.hpp"

using namespace agent_framework;

Bmc::Bmc(const ConnectionData& conn, Bmc::Duration state_update_interval,
         ReadPresenceFn read_presence, ReadOnlineStatusFn read_online_state)
    : BmcStateMachine(conn.get_ip_address() + ":" + std::to_string(conn.get_port()), read_presence, read_online_state),
      m_connection_data{conn},
      m_state_update_interval{state_update_interval},
      m_worker{conn.get_ip_address()} {
}

Bmc::~Bmc() {
    stop();
}

void Bmc::start() {
    reset_tasks();
}

void Bmc::stop() {
    m_worker.stop();
    log_debug("bmc", get_id() << " stopped ");
}

void Bmc::reset_tasks() {
    m_worker.remove_tasks();
    periodic_task("state_update", Duration::zero(), m_state_update_interval,
            [this](){ update_state(); });
}
