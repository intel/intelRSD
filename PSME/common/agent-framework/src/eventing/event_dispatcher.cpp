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
#include "agent-framework/module/requests/psme/component_notification.hpp"
#include "json-rpc/connectors/http_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include <string>

using namespace std;
using namespace agent_framework::generic;
using agent_framework::eventing::ComponentNotification;

namespace {
const size_t QUEUE_WAIT_TIME = 1000;
}

namespace agent_framework {
namespace generic {

class EventSender final {
public:
    EventSender() :
        m_connector(new json_rpc::HttpClientConnector({})),
        m_invoker(new json_rpc::JsonRpcRequestInvoker())
        {}

    explicit EventSender(const std::string& url) :
        m_url{url},
        m_connector(new json_rpc::HttpClientConnector(url)),
        m_invoker(new json_rpc::JsonRpcRequestInvoker()),
        m_is_enabled(true) {}

    void send_notifications(ComponentNotification&& notification) {
        /* is_enabled lock should NOT protect sending a message: in case of timeout
         * it makes the thread troubles (event collection grows infinitly)
         */
        if (!send_enabled()) {
            log_warning("eventing", "Discard notifications, sending not enabled");
            return;
        }

        const auto& gami_id = ServiceUuid::get_instance()->get_service_uuid();
        notification.set_gami_id(gami_id);

        agent_framework::model::requests::ComponentNotification rpc_notification{};
        rpc_notification.set_notifications(notification.get_notifications());
        rpc_notification.set_gami_id(notification.get_gami_id());

        try {
            json::Json json = rpc_notification.to_json();
            log_debug("eventing", "Sending notification: " << json);
            m_invoker->prepare_notification("componentNotification", json);
            m_invoker->call(m_connector);
        }
        catch (const json_rpc::JsonRpcException& e) {
            log_error("eventing", "send_notification error: " << e.what());
        }
    }

    bool send_enabled() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_is_enabled;
    }

    void enable_send(const std::string& url) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_url = url;
        m_connector->set_url(url);
        m_is_enabled = true;
    }

    void disable_send() {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_url.clear();
        m_connector->set_url(m_url);
        m_is_enabled = false;
    }

private:
    std::string m_url{};
    mutable std::mutex m_mutex{};
    std::shared_ptr<json_rpc::HttpClientConnector> m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestInvoker> m_invoker{};
    bool m_is_enabled{false};
};

}
}

EventDispatcher::EventDispatcher() : m_event_sender{new EventSender()} {}

EventDispatcher::~EventDispatcher() {}

void EventDispatcher::enable_send_notifications(const std::string& url) {
    m_event_sender->enable_send(url);
    log_info("eventing", "Sending AMC notifications enabled.");
}

void EventDispatcher::disable_send_notifications() {
    m_event_sender->disable_send();
    log_info("eventing", "Sending AMC notifications disabled.");
}

bool EventDispatcher::send_notifications_enabled() const {
    return m_event_sender->send_enabled();
}

void EventDispatcher::execute() {
    using agent_framework::eventing::EventsQueue;
    log_info("eventing", "Starting EventDispatcher thread...");

    while (is_running()) {
        auto notification = EventsQueue::get_instance()->wait_for_and_pop(chrono::milliseconds(QUEUE_WAIT_TIME));

        if (notification) {
            if (notification->get_notifications().empty()) {
                log_debug("eventing", "Discarding empty notification");
                continue;
            }

            m_event_sender->send_notifications(std::move(*notification));
        }
    }
    log_info("eventing", "EventDispatcher thread stopped.");
}
