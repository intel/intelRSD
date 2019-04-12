/*!
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
 * */

#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/psme_commands.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "psme/registration/registration_server.hpp"

#include <chrono>

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace psme::core::agent;

namespace {

uint64_t get_time() {
    auto time_clock = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(time_clock).count();
}

static constexpr unsigned MINIMUM_DELAY = 3;

void handle_heart_beat(const HeartBeat::Request& request, HeartBeat::Response& response) {

    auto agent = AgentManager::get_instance()->get_agent(request.get_gami_id());
    if (agent) {
        static uint64_t start_time = get_time();
        response.set_time_stamp(get_time() - start_time);
    }
    else {
        response.set_time_stamp(0);
    }
    response.set_min_delay(MINIMUM_DELAY);
}

}

REGISTER_COMMAND(HeartBeat, ::handle_heart_beat);
