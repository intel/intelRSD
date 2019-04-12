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
 *
 * @file event.hpp
 * @brief Declaration of Event class for PSME eventing.
 *
 * This class represents Event.EventRecord metadata EntityType
 * */

#pragma once



#include "agent-framework/module/enum/enum_builder.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>


namespace psme {
namespace rest {
namespace eventing {

/*!
 * @brief EventType Type of event
 */
ENUM(EventType, uint32_t,
     StatusChange,
     ResourceUpdated,
     ResourceAdded,
     ResourceRemoved,
     Alert
);

/*!
 * @brief Event class
 * */
class Event {
public:
    /*!
     * Constructor
     *
     * @param type Event type
     * @param origin_of_condition Origin of condition
     */
    Event(EventType type, const std::string& origin_of_condition);


    /*!
     * @brief Return event type
     *
     * @return Event type
     */
    EventType get_type() const {
        return m_type;
    }


    /*!
     * @brief Set event type
     *
     * @param type Event type
     */
    void set_type(EventType type) {
        m_type = type;
    }


    /*!
     * @brief Get event id
     *
     * @return Event id
     */
    const std::string& get_event_id() const {
        return m_event_id;
    }


    /*!
     * @brief Set event id
     *
     * @param event_id Event id
     */
    void set_event_id(const std::string& event_id) {
        m_event_id = event_id;
    }


    /*!
     * @brief Get timestamp
     *
     * @return Timestamp
     */
    const std::chrono::steady_clock::time_point& get_timestamp() const {
        return m_timestamp;
    }


    /*!
     * @brief Get severity
     *
     * @return Severity
     */
    const std::string& get_severity() const {
        return m_severity;
    }


    /*!
     * @brief Set severity
     *
     * @param severity Severity
     */
    void set_severity(const std::string& severity) {
        m_severity = severity;
    }


    /*!
     * @brief Get message
     *
     * @return Message
     */
    const std::string& get_message() const {
        return m_message;
    }


    /*!
     * @brief Set message
     *
     * @param message Message
     */
    void set_message(const std::string& message) {
        m_message = message;
    }


    /*!
     * @brief Get message id
     *
     * @return Message id
     */
    const std::string& get_message_id() const {
        return m_message_id;
    }


    /*!
     * @brief Set message id
     *
     * @param message_id Message id
     */
    void set_message_id(const std::string& message_id) {
        m_message_id = message_id;
    }


    /*!
     * @brief Get origin of condition
     *
     * @return Origin of condition
     */
    const std::string& get_origin_of_condition() const {
        return m_origin_of_condition;
    }


    /*!
     * @brief Set origin of condition
     *
     * @param origin_of_condition Origin of condition
     */
    void set_origin_of_condition(const std::string& origin_of_condition) {
        m_origin_of_condition = origin_of_condition;
    }


    /*!
     * @brief Get message args
     *
     * @return Message args
     */
    const std::vector<std::string>& get_message_args() const {
        return m_message_args;
    }


    /*!
     * @brief Add message arg
     *
     * @param message_arg Message arg
     */
    void add_message_arg(const std::string& message_arg) {
        m_message_args.emplace_back(message_arg);
    }


    /*!
     * @brief Convert Event to JSON representation
     *
     * @return JSON representation
     */
    json::Json to_json() const;


private:
    EventType m_type;
    std::string m_event_id{};
    std::chrono::steady_clock::time_point m_timestamp{std::chrono::steady_clock::now()};
    // @TODO: reimplement message handling
    std::string m_severity{"OK"};
    std::string m_message{"Successfully Completed Request"};
    std::string m_message_id{"Base.1.0.0.Success"};
    std::vector<std::string> m_message_args{};
    std::string m_origin_of_condition;
};

/*! Event Vector type */
using EventVec = std::vector<Event>;

}
}
}


