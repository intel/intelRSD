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
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;

REGISTER_COMMAND(ComponentNotification,
    [] (const ComponentNotification::Request& request, ComponentNotification::Response&) {
        log_debug("rest",
                  "Got component notification with " + std::to_string(request.get_notifications().size()) + " events.");

        agent_framework::eventing::EventsQueue::get_instance()->push_back(request);
    }
);
