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
 * @file event_service.cpp
 *
 * @brief EventService implementation
 *
 * */

#include "psme/rest/eventing/event_service.hpp"
#include "psme/rest/http/http_defs.hpp"
#include "psme/rest/eventing/rest_client.hpp"
#include "psme/rest/eventing/config/subscription_config.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "configuration/configuration.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::config;
using namespace psme::rest::eventing::manager;

constexpr char EventService::DELIVERY_RETRY_ATTEMPTS_PROP[];
constexpr char EventService::DELIVERY_RETRY_INTERVAL_PROP[];

EventService::EventService() {
    const json::Value& config =
            configuration::Configuration::get_instance().to_json();
    const auto& event_service_config = config["event-service"];
    m_delivery_retry_attempts =
            event_service_config[DELIVERY_RETRY_ATTEMPTS_PROP].as_uint();
    m_delivery_retry_interval =
            std::chrono::seconds(
                event_service_config[DELIVERY_RETRY_INTERVAL_PROP].as_uint());
}

void EventService::start() {
    log_info(GET_LOGGER("rest"), "Starting REST event service ...");
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&EventService::m_handle_events, this);
        log_info(GET_LOGGER("rest"), "REST event service started.");
    }
}

void EventService::stop() {
    log_info(GET_LOGGER("rest"), "Stopping REST event service ...");
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
        log_info(GET_LOGGER("rest"), "REST event service stopped.");
    }
}

EventService::~EventService() {
    stop();
}

void EventService::post_event(EventUPtr event, const steady_clock::duration& delay) {
    log_debug(GET_LOGGER("rest"), "Post event :" << event->to_json());
    return EventService::get_event_queue().push_back(std::move(event), delay);
}

EventQueue& EventService::get_event_queue() {
    static EventQueue g_event_queue;
    return g_event_queue;
}

namespace {
    json::Value create_notification(json::Value event_as_json) {
        json::Value notification;
        notification["@odata.context"] = "/rest/v1/$metadata#EventService/Members/Events/1";
        notification["@odata.id"] = "/rest/v1/EventService/Events/1";
        notification["@odata.type"] = "#EventService.1.0.0.Event";
        notification["Id"] = "1";
        notification["Name"] = "Event Array";
        notification["Description"] = "Events";
        notification["Events"].push_back(std::move(event_as_json));
        return notification;
    }

    void send_event(const EventService& event_service,
                    Event event,
                    const Subscription& subscription) {
        const auto& destination = subscription.get_destination();
        const auto& context = subscription.get_context();
        event.set_subscriber_id(std::to_string(subscription.get_id()));
        event.set_context(context);
        try {
            std::string notification =
                    json::Serializer(create_notification(event.to_json()));
            psme::rest::eventing::RestClient rest_client("");
            rest_client.set_default_content_type(psme::rest::http::MimeType::JSON);
            rest_client.post(destination, notification);
            log_debug(GET_LOGGER("rest"), " Subscriber: " << destination
                                        << " notified with: " << notification);
        } catch (std::runtime_error&) {
            EventUPtr retry_event(new Event(event));
            auto retry_attempts = retry_event->increment_retry_attempts();
            if (retry_attempts < event_service.get_delivery_retry_attempts()) {
                log_warning(GET_LOGGER("rest"), "Failed to send event with EventId: "
                    << retry_event->get_event_id() << " to: " << destination
                    << " retry attempt no: " << retry_attempts);
                EventService::post_event(std::move(retry_event),
                                    event_service.get_delivery_retry_interval());
            }
            else {
                log_warning(GET_LOGGER("rest"), "EventId: " << event.get_event_id()
                        << " could not be delivered: "
                        << destination << " is unreachable");
                SubscriptionManager::get_instance()->del(subscription.get_name());
                SubscriptionConfig::get_instance()->save();
            }
        }
    }
}

SubscriptionVec EventService::select_subscribers(const Event& event) {
    SubscriptionVec subscribers{};
    for (const auto& item : SubscriptionManager::get_instance()->get()) {
        const auto& subscription = item.second;
        for (const auto& event_type: subscription.get_event_types().get()) {
            if (event_type == event.get_type()) {
                subscribers.emplace_back(subscription);
            }
        }
    }
    return subscribers;
}

void EventService::m_handle_events() {
    while (m_running) {
        if (const auto event =
                get_event_queue().wait_for_and_pop(std::chrono::seconds(1))) {

            log_debug(GET_LOGGER("rest"), " Popped EVENT: "
                        << json::Serializer(event->to_json()));

            try {
                auto subscribers = select_subscribers(*event);

                for (const auto& subscriber: subscribers) {
                    // @TODO send_event as tasks processed by threadpool
                    send_event(*this, *event, subscriber);
                }
            }
            catch (const std::runtime_error& e) {
                log_error(GET_LOGGER("rest"),
                    " Exception occured when processing event: "
                        << event->to_json() << " : " << e.what());
            }
            catch (...) {
                log_error(GET_LOGGER("rest"),
                    " Exception occured when processing event: "
                        << event->to_json());
            }
        }
    }
}
