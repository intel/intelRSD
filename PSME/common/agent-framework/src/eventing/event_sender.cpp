/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file event_sender.cpp
 * @brief Definition of Event Sender class
 * */

#include "agent-framework/eventing/event_sender.hpp"
#include "agent-framework/module/service_uuid.hpp"



using namespace agent_framework::generic;


EventSender::EventSender() :
    m_connector(new json_rpc::HttpClientConnector({})),
    m_invoker(new json_rpc::JsonRpcRequestInvoker()) {}


EventSender::EventSender(const std::string& url) :
    m_url{url},
    m_connector(new json_rpc::HttpClientConnector(url)),
    m_invoker(new json_rpc::JsonRpcRequestInvoker()),
    m_is_enabled(true) {}

void EventSender::send_notifications(model::requests::ComponentNotification&& notification) {
    /*
     * is_enabled lock should NOT protect sending a message: in case of timeout
     * it makes the thread troubles (event collection grows infinitely)
     */
    if (!send_enabled()) {
        log_warning("eventing", "Discard notifications, sending not enabled");
        return;
    }

    const auto& gami_id = module::ServiceUuid::get_instance()->get_service_uuid();
    notification.set_gami_id(gami_id);

    try {
        json::Json json = notification.to_json();
        log_debug("eventing", "Sending notification: " << json);
        m_invoker->prepare_notification(model::literals::Command::COMPONENT_NOTIFICATION, json);
        m_invoker->call(m_connector);
    }
    catch (const json_rpc::JsonRpcException& e) {
        log_error("eventing", "Send notification error: " << e.what());
    }
}


bool EventSender::send_enabled() const {
    std::lock_guard<std::mutex> lk(m_mutex);
    return m_is_enabled;
}

void EventSender::enable_send(const std::string& url) {
    std::lock_guard<std::mutex> lk(m_mutex);
    m_url = url;
    m_connector->set_url(url);
    m_is_enabled = true;
}

void EventSender::disable_send() {
    std::lock_guard<std::mutex> lk(m_mutex);
    m_url.clear();
    m_connector->set_url(m_url);
    m_is_enabled = false;
}