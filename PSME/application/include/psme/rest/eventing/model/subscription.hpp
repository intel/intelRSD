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
 * */

#pragma once
#include "agent-framework/module/enum/enum_builder.hpp"
#include "psme/rest/eventing/event.hpp"
#include <string>
#include <vector>

namespace psme {
namespace rest {
namespace eventing {
namespace model {


/*!
 * @brief SubscriptionProtocol allowable Protocols describing form of events sent to subscriber.
 */
ENUM(SubscriptionProtocol, uint32_t, Redfish);


/*!
 * @brief EventTypes class
 */
class EventTypes {
public:
    /*!
     * @brief EventTypeVec EventType Vector type
     */
    using EventTypeVec = std::vector<EventType>;
    /*!
     * @brief Add event type
     *
     * @param event_type Event type
     */
    void add(EventType event_type);

    /*!
     * @brief Get event type collection
     */
    EventTypeVec get() const;

private:
    EventTypeVec m_event_types{};
};

/*!
 * @brief Subscription representation
 */
class Subscription final {
public:
    /*!
     * @brief Set subscription id
     *
     * @param id Subscription id
     */
    void set_id(uint64_t id) {
        m_id = id;
    }

    /*!
     * @brief Get subscription id
     *
     * @return subscription id
     */
    uint64_t get_id() const {
        return m_id;
    }

    /*!
     * @brief Set subscription name
     *
     * @param name Subscription name
     */
    void set_name(const std::string& name) {
        m_name = name;
    }

    /*!
     * @brief Get subscription name
     *
     * @return subscription name
     */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Set subscription destination
     *
     * @param destination Subscription destination
     */
    void set_destination(const std::string& destination) {
        m_destination = destination;
    }

    /*!
     * @brief Get subscription destination
     *
     * @return Subscription destination
     */
    const std::string& get_destination() const {
        return m_destination;
    }

    /*!
     * @brief Set subscription event types
     *
     * @param event_types Subscription event types
     */
    void set_event_types(const EventTypes& event_types) {
        m_event_types = event_types;
    }

    /*!
     * @brief Get subscription event types
     *
     * @return Subscription event types
     */
    const EventTypes& get_event_types() const {
        return m_event_types;
    }

    /*!
     * @brief Set subscription context
     *
     * @param context Subscription context
     */
    void set_context(const std::string& context) {
        m_context= context;
    }

    /*!
     * @brief Get subscription context
     *
     * @return Subscription context
     */
    const std::string& get_context() const {
        return m_context;
    }

    /*!
     * @brief Set subscription protocol
     *
     * @param protocol Subscription protocol
     */
    void set_protocol(const SubscriptionProtocol protocol) {
        m_protocol = protocol;
    }

    /*!
     * @brief Get subscription protocol
     *
     * @return Subscription protocol
     */
    const SubscriptionProtocol get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Get list of specified Event origin resources
     * @return list of specified Event origin resources
     * */
    const std::vector<std::string>& get_origin_resources() const {
        return m_origin_resources;
    }


    /*!
     * @brief Set list of specified Event origin resources
     * @param[in] origin_resources the list of specified Event origin resources
     * */
    void set_origin_resources(const std::vector<std::string>& origin_resources) {
        m_origin_resources = origin_resources;
    }


    /*!
     * @brief Add member to list of specified Event origin resources
     * @param[in] origin_resource the member to add to list of specified Event origin resources
     * */
    void add_origin_resource(const std::string& origin_resource) {
        m_origin_resources.push_back(origin_resource);
    }


    /*!
     * @brief Checks it event is requested by subscription
     *
     * @param event Checked event
     * @return True it event should be sent to subscription
     */
    bool is_subscribed_for(const Event& event) const;

    /*!
     * @brief Creates json representation of subscription
     *
     * @return JSON representation of subscription
     */
    json::Json to_json() const;

    /*!
     * @brief Fills json with representation of subscription
     *
     * @param json JSON to be filled with representation of subscription
     */
    void fill_json(json::Json& json) const;

    /*!
     * @brief Creates model representation from subscription JSON
     *
     * @param json JSON representation of subscription
     * @param validate_origin_resources flag determining if URL's of origin resources should be verified.
     * It should be false when subscriptions are read from file at start-up,
     * because in the start-up sequence endpoints are registered in the Multiplexer AFTER the event service starts.
     * @return Model representation of subscription
     */
    static Subscription from_json(const json::Json& json, bool validate_origin_resources = true);

private:
    uint64_t m_id{};
    std::string m_name{};
    std::string m_destination{};
    EventTypes m_event_types{};
    std::string m_context{};
    SubscriptionProtocol m_protocol{SubscriptionProtocol::Redfish};
    std::vector<std::string> m_origin_resources{};
};

/*!
 * @return true if Subscriptions are equal (except id field)
 * @param s1 First Subscription
 * @param s2 Second Subscription
 */
bool equal_subscriptions(const Subscription& s1, const Subscription& s2);

}
}
}
}
