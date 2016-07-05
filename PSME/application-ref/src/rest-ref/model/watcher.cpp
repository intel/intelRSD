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
 * */


#include "psme/rest-ref/model/watcher.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/rest-ref/endpoints/endpoint_builder.hpp"
#include "psme/core/agent/agent_manager.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "json/json.hpp"
#include "configuration/configuration.hpp"

#include <iostream>
#include <chrono>

namespace psme {
namespace rest {
namespace model {
using namespace std;

using agent_framework::eventing::EventsQueue;

Watcher::~Watcher() { stop(); }

void Watcher::start() {
    auto config = configuration::Configuration::get_instance().to_json();
    m_interval = std::chrono::seconds(config["eventing"]["poll-interval-sec"].as_uint());
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&Watcher::watch, this);
    }
}

void Watcher::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
            log_debug(GET_LOGGER("rest"), "Watcher job done!");
        }
    }
}

void Watcher::schedule_polling() {
    m_next_polling = std::chrono::system_clock::now() + m_interval;
    log_debug(GET_LOGGER("rest"), "Polling starts in " << m_interval.count() << " seconds");
}

void Watcher::watch() {
    schedule_polling();

    while (m_running) {
        if (std::chrono::system_clock::now() >= m_next_polling) {
            // polling will re-read everytning so we do not need events that arrived before polling.
            EventsQueue::get_instance()->clear();
            try {
                poll();
            }
            catch (...) {
                log_error(GET_LOGGER("rest"), "Exception while executing polling");
            }
            schedule_polling();
        }

        auto event = EventsQueue::get_instance()->
                wait_for_and_pop(std::chrono::seconds(1));

        if (event && m_running) {
            process_event(event.get());
        }
    }
}

void Watcher::process_event(const EventData *const event) {
    try {
        log_info(GET_LOGGER("rest"), "Start processing event!");
        auto agent = core::agent::AgentManager::get_instance()->get_agent(event->get_gami_id());
        if(nullptr == agent) {
            log_error(GET_LOGGER("rest"), "Agent GAMI ID " << event->get_gami_id() << " not recognized");
            return;
        }
        auto handler = handler::HandlerManager::get_instance()->get_handler(event->get_type());

        if (!handler->handle(agent, *event)) {
            log_info(GET_LOGGER("rest"), "Event not processed corectly. event->get_type() = " << event->get_type()
                                          << "; event->get_notification() = " << event->get_notification().to_string());
        }
    } catch (const std::exception &error) {
        log_error(GET_LOGGER("rest"),
                  "Event exception occured: " << error.what());
    } catch (...) {
        log_error(GET_LOGGER("rest"),
                  "Unknown exception occured.");
    }
}

void Watcher::poll() {
    auto started_at = std::chrono::high_resolution_clock::now();

    log_info(GET_LOGGER("rest"), "Polling started");
    auto agents = core::agent::AgentManager::get_instance()->get_agents();
    for (auto& agent : agents) {
        try {
            m_root_handler.poll(agent, "" /* parent_uuid */, "" /* uuid */);
        }
        catch (const core::agent::AgentUnreachable&)  {
            log_error(GET_LOGGER("rest"), "Polling failed due to agent (id:" << agent.get()->get_gami_id() << ") error");
        }
    }

    auto finished_at = std::chrono::high_resolution_clock::now();
    auto duration = finished_at - started_at;
    log_info(GET_LOGGER("rest"), "Polling completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms");
}

}
}
}
