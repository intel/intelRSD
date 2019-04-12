/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
#include "psme/rest/eventing/rest_client.hpp"
#include "psme/rest/eventing/model/subscription.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/server/content_types.hpp"
#include "configuration/configuration.hpp"

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::eventing::model;

constexpr char EventService::DELIVERY_RETRY_ATTEMPTS_PROP[];
constexpr char EventService::DELIVERY_RETRY_INTERVAL_PROP[];

EventService::EventService() {
    const json::Json& config = configuration::Configuration::get_instance().to_json();
    const auto& event_service_config = config["event-service"];
    m_delivery_retry_attempts = event_service_config.value(DELIVERY_RETRY_ATTEMPTS_PROP, std::uint16_t{});
    m_delivery_retry_interval = std::chrono::seconds(event_service_config.value(DELIVERY_RETRY_INTERVAL_PROP, std::uint16_t{}));
}

void EventService::start() {
    log_info("rest", "Starting REST event service ...");
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&EventService::m_handle_events, this);
        log_info("rest", "REST event service started.");
    }
}

void EventService::stop() {
    log_info("rest", "Stopping REST event service ...");
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
        log_info("rest", "REST event service stopped.");
    }
}

EventService::~EventService() {
    stop();
}

void EventService::post_events_array(EventArrayUPtr event_array, const steady_clock::duration& delay) {
    log_debug("rest", "Post events :" << event_array->to_json());
    return EventService::get_event_array_queue().push_back(std::move(event_array), delay);
}

EventArrayQueue& EventService::get_event_array_queue() {
    static EventArrayQueue g_event_queue;
    return g_event_queue;
}

void EventService::send_event_array(const EventArray& event_array) {
    Subscription subscription;
    try {
        subscription =  SubscriptionManager::get_instance()->get(event_array.get_subscriber_id());
    }
    catch (const agent_framework::exceptions::NotFound&) {
        // The subscriber has been deleted while the EventArray waited in the queue for processing. Do nothing.
        return;
    }

    const auto& destination = subscription.get_destination();
    try {
        std::string notification = event_array.to_json().dump();
        psme::rest::eventing::RestClient rest_client("");
        rest_client.set_default_content_type(psme::rest::server::ContentType::JSON);
        rest_client.post(destination, notification);
        log_debug("rest", " Subscriber: " << destination
                                    << " notified with: " << notification);
    } catch (std::runtime_error&) {
        EventArrayUPtr retry_event_array(new EventArray(event_array));
        auto retry_attempts = retry_event_array->increment_retry_attempts();
        if (retry_attempts < this->m_delivery_retry_attempts) {
            log_warning("rest", "Failed to send event array with Id: "
                << retry_event_array->get_id() << " to: " << destination
                << " retry attempt no: " << retry_attempts);
            EventService::post_events_array(std::move(retry_event_array), this->m_delivery_retry_interval);
        }
        else {
            log_warning("rest", "Event array with Id: " << event_array.get_id()
                    << " could not be delivered: "
                    << destination << " is unreachable");
            SubscriptionManager::get_instance()->del(subscription.get_id());
        }
    }
}

std::vector<EventArray> EventService::select_events_for_subscribers(const EventArray& event_array) {
    std::vector<EventArray> selections{};
    SubscriptionManager::get_instance()->for_each([&selections, &event_array](const Subscription& subscription) {
        EventVec events{};
        std::copy_if(event_array.get_events().cbegin(), event_array.get_events().cend(), std::back_inserter(events),
            [&subscription] (const Event& event) -> bool { return subscription.is_subscribed_for(event); });

        if (!events.empty()) {
            EventArray selection{std::move(events)};
            selection.set_context(subscription.get_context());
            selection.set_subscriber_id(subscription.get_id());
            // EventArray is ready for POSTing - assign it a Redfish Id.
            selection.assign_new_id();
            selections.emplace_back(std::move(selection));
        }
    });
    return selections;
}

void EventService::m_handle_events() {
    while (m_running) {
        if (const auto event_array = get_event_array_queue().wait_for_and_pop(std::chrono::seconds(1))) {

            log_debug("rest", " Popped Event Array: "
                        << event_array->to_json().dump());

            try {
                if (!event_array->get_subscriber_id().has_value()) {
                    // event array is processed for the first time. For each subscriber,
                    // create an EventArray containing only the events that match the subscription
                    auto filtered_event_arrays = select_events_for_subscribers(*event_array);

                    for (auto& events_for_subscriber : filtered_event_arrays) {
                        // @TODO parallelize sending event array by using tasks processed by a threadpool
                        send_event_array(events_for_subscriber);
                    }
                } else {
                    send_event_array(*event_array);
                }
            }
            catch (const std::runtime_error& e) {
                log_error("rest",
                    " Exception occurred when processing event array: "
                        << event_array->to_json() << " : " << e.what());
            }
            catch (...) {
                log_error("rest",
                    " Exception occurred when processing event array: "
                        << event_array->to_json());
            }
        }
    }
}
