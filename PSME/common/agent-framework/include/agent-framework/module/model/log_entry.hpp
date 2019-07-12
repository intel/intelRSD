/*!
 * @brief Log entry model
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file log_entry.hpp
 */

#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/sensor_type.hpp"
#include "agent-framework/module/enum/entry_code.hpp"


namespace agent_framework {
namespace model {

/*! LogEntry */
class LogEntry : public Resource {
public:
    explicit LogEntry(const std::string &parent_uuid = {}, enums::Component parent_type = enums::Component::LogService);


    ~LogEntry();


    LogEntry(const LogEntry&) = default;


    LogEntry(LogEntry &&) = default;


    LogEntry &operator=(const LogEntry &) = default;


    LogEntry &operator=(LogEntry &&) = default;


    /*!
     * @brief construct an object of class LogEntry from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed LogEntry object
     */
    static LogEntry from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSON
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name.
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return LogEntry::collection_name;
    }


    /*!
     * @brief Get component name.
     * @return component name
     */
    static enums::Component get_component() {
        return LogEntry::component;
    }


    /*!
     * @brief Set entry creation time.
     * @param[in] created entry created time
     * */
    void set_created(const OptionalField<std::string>& created) {
        m_created = created;
    }


    /*!
     * @brief Get entry creation time.
     * @return created time
     * */
    const OptionalField<std::string>& get_created() const {
        return m_created;
    }


    /*!
     * @brief Set entry code.
     * @param[in] entry_code entry code
     * */
    void set_entry_code(const OptionalField<enums::EntryCode>& entry_code) {
        m_entry_code = entry_code;
    }


    /*!
     * @brief Get entry code.
     * @return entry code
     * */
    const OptionalField<enums::EntryCode>& get_entry_code() const {
        return m_entry_code;
    }


    /*!
     * @brief Set entry type.
     * @param[in] entry_type entry type
     * */
    void set_entry_type(const OptionalField<enums::EntryType>& entry_type) {
        m_entry_type = entry_type;
    }


    /*!
     * @brief Get entry type.
     * @return entry type
     * */
    const OptionalField<enums::EntryType>& get_entry_type() const {
        return m_entry_type;
    }


    /*!
     * @brief Set event id.
     * @param[in] event_id event id
     * */
    void set_event_id(const OptionalField<std::uint32_t>& event_id) {
        m_event_id = event_id;
    }


    /*!
     * @brief Get event id.
     * @return event id
     * */
    const OptionalField<std::uint32_t>& get_event_id() const {
        return m_event_id;
    }


    /*!
    * @brief Set event timestamp.
    * @param[in] event_timestamp event timestamp
    * */
    void set_event_timestamp(const OptionalField<std::string>& event_timestamp) {
        m_event_timestamp = event_timestamp;
    }


    /*!
     * @brief Get event timestamp.
     * @return event timestamp
     * */
    const OptionalField<std::string>& get_event_timestamp() const {
        return m_event_timestamp;
    }


    /*!
    * @brief Set event severity.
    * @param[in] severity event severity
    * */
    void set_severity(const OptionalField<enums::Health>& severity) {
        m_severity = severity;
    }


    /*!
     * @brief Get event severity.
     * @return event severity
     * */
    const OptionalField<enums::Health>& get_severity() const {
        return m_severity;
    }


    /*!
    * @brief Set sensor type.
    * @param[in] sensor type
    * */
    void set_sensor_type(const OptionalField<enums::SensorType>& sensor_type) {
        m_sensor_type = sensor_type;
    }


    /*!
     * @brief Get sensor type.
     * @return sensor type
     * */
    const OptionalField<enums::SensorType>& get_sensor_type() const {
        return m_sensor_type;
    }


    /*!
    * @brief Set sensor number.
    * @param[in] sensor_number sensor number
    * */
    void set_sensor_number(const OptionalField<std::uint32_t>& sensor_number) {
        m_sensor_number = sensor_number;
    }


    /*!
     * @brief Get sensor number
     * @return sensor number
     * */
    const OptionalField<std::uint32_t>& get_sensor_number() const {
        return m_sensor_number;
    }


    /*!
    * @brief Set Event type.
    * @param[in] event type
    * */
    void set_event_type(const OptionalField<enums::EventType>& event_type) {
        m_event_type = event_type;
    }


    /*!
     * @brief Get event type.
     * @return event type
     * */
    const OptionalField<enums::EventType>& get_event_type() const {
        return m_event_type;
    }


    /*!
    * @brief Set message.
    * @param[in] message
    * */
    void set_message(const OptionalField<std::string>& message) {
        m_message = message;
    }


    /*!
     * @brief Get message.
     * @return message
     * */
    const OptionalField<std::string>& get_message() const {
        return m_message;
    }


    /*!
    * @brief Set message id.
    * @param[in] message id
    * */
    void set_message_id(const OptionalField<std::string>& message_id) {
        m_message_id = message_id;
    }


    /*!
     * @brief Get message id.
     * @return message id
     * */
    const OptionalField<std::string>& get_message_id() const {
        return m_message_id;
    }


    /*!
    * @brief Set record id.
    * @param[in] record id
    * */
    void set_record_id(const OptionalField<std::uint32_t>& record_id) {
        m_record_id = record_id;
    }


    /*!
     * @brief Get origin of condition.
     * @return origin of condition
     * */
    const OptionalField<Uuid>& get_origin_of_condition() const {
        return m_origin_of_condition;
    }


    /*!
     * @brief Get record id.
     * @return record id
     * */
    const OptionalField<std::uint32_t>& get_record_id() const {
        return m_record_id;
    }


    /*!
     * @brief Set origin of condition.
     * @param[in] origin_of_condition origin of condition
     * */
    void set_origin_of_condition(const OptionalField<Uuid>& origin_of_condition) {
        m_origin_of_condition = origin_of_condition;
    }

private:
    OptionalField<std::string> m_created{};
    OptionalField<enums::EntryType> m_entry_type{};
    OptionalField<enums::EventType> m_event_type{};
    OptionalField<std::uint32_t> m_event_id{};
    OptionalField<std::string> m_event_timestamp{};
    OptionalField<enums::Health> m_severity{};
    OptionalField<std::string> m_message{};
    OptionalField<std::string> m_message_id{};

    /* SEL specific entries */
    OptionalField<enums::EntryCode> m_entry_code{};
    OptionalField<enums::SensorType> m_sensor_type{};
    OptionalField<std::uint32_t> m_sensor_number{};
    OptionalField<std::uint32_t> m_record_id{};
    OptionalField<Uuid> m_origin_of_condition{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};


}
}
