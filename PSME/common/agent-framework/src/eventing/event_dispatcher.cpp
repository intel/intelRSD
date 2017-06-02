/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
*/
#include "agent-framework/eventing/event_dispatcher.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/service_uuid.hpp"

#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <string>

using namespace std;
using namespace agent_framework::generic;
using agent_framework::eventing::EventData;

namespace {
const size_t QUEUE_WAIT_TIME = 1000;
}

namespace agent_framework {
namespace generic {

class EventSender final {
public:
    explicit EventSender(const std::string& url = "") :
        m_url{url},
        m_http_connector(new jsonrpc::HttpClient(m_url)),
        m_jsonrpc_client(new jsonrpc::Client(*m_http_connector)) {}

    void send_event(EventData& event) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_url.empty()) {
            return;
        }

        try {
            event.set_gami_id(ServiceUuid::get_instance()->get_service_uuid());
            m_jsonrpc_client->CallNotification("componentNotification", event.to_json());
        } catch (const jsonrpc::JsonRpcException& e) {
            log_error(GET_LOGGER("eventing"), "send_event error: " << e.what());
        }
    }

    bool send_enabled() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        return !m_url.empty();
    }

    void enable_send(const std::string& url) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_url = url;
        m_http_connector->SetUrl(m_url);
    }

    void disable_send() {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_url.clear();
        m_http_connector->SetUrl(m_url);
    }

private:
    std::string m_url{};
    mutable std::mutex m_mutex{};
    std::unique_ptr<jsonrpc::HttpClient> m_http_connector{};
    std::unique_ptr<jsonrpc::Client> m_jsonrpc_client{};
};

}
}

EventDispatcher::EventDispatcher() : m_event_sender{new EventSender()} {}

EventDispatcher::~EventDispatcher() {}

void EventDispatcher::enable_send_notifications(const std::string& url) {
    m_event_sender->enable_send(url);
    log_info(GET_LOGGER("eventing"), "Sending AMC notifications enabled.");
}

void EventDispatcher::disable_send_notifications() {
    m_event_sender->disable_send();
    log_info(GET_LOGGER("eventing"), "Sending AMC notifications disabled.");
}

bool EventDispatcher::send_notifications_enabled() const {
    return m_event_sender->send_enabled();
}

void EventDispatcher::execute() {
    using agent_framework::eventing::EventsQueue;
    log_info(GET_LOGGER("eventing"), "Starting EventDispatcher thread...");

    while (is_running()) {
        auto event = EventsQueue::get_instance()->wait_for_and_pop(chrono::milliseconds(QUEUE_WAIT_TIME));

        if (event) {
            log_debug(GET_LOGGER("eventing"), "Popped event: " << event->to_json().toStyledString());

            m_event_sender->send_event(*event);
        }
    }
    log_info(GET_LOGGER("eventing"), "EventDispatcher thread stopped.");
}
