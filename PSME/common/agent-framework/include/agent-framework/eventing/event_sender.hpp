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
 * @file event_sender.hpp
 * @brief Declaration of Event Sender class
 * */

#pragma once



#include "agent-framework/module/requests/psme/component_notification.hpp"

#include "json-rpc/connectors/http_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include <string>
#include <mutex>


namespace agent_framework {
namespace generic {

class EventSender final {
public:
    EventSender();

    explicit EventSender(const std::string& url);

    void send_notifications(model::requests::ComponentNotification&& notification);

    bool send_enabled() const;

    void enable_send(const std::string& url);

    void disable_send();

private:
    std::string m_url{};
    mutable std::mutex m_mutex{};
    std::shared_ptr<json_rpc::HttpClientConnector> m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestInvoker> m_invoker{};
    bool m_is_enabled{false};
};

}
}