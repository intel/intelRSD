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
 * */

#include "agent-framework/logger_ext.hpp"
#include "psme/command/eventing/definition/tag.hpp"
#include "psme/command/eventing/eventing_agent.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace psme;
using namespace psme::command;
//using namespace psme::app;

/*! EventingAgent implementation */
class EventingAgent : public psme::command::eventing::EventingAgent {
public:
    EventingAgent() : psme::command::eventing::EventingAgent(
            psme::command::eventing::definition::TAG) { }

    using psme::command::eventing::EventingAgent::execute;

    void execute(const Request& request, Response&) {

        using agent_framework::eventing::EventsQueue;
        using agent_framework::eventing::EventData;
        EventData event{};
        event.set_gami_id(request.get_gami_id());
        event.set_component(request.get_component());
        event.set_notification(request.get_notification());
        event.set_parent(request.get_parent());
        event.set_type(request.get_type());

        log_debug(GET_LOGGER("rest"), "GET EVENT REQUEASTEEEE");

        EventsQueue::get_instance()->push_back(std::move(event));
    }

    ~EventingAgent();
};

EventingAgent::~EventingAgent() { }

static Command::Register<EventingAgent> g;
