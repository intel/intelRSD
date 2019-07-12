/*!
 * @brief Log service model
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
 * @file log_service.hpp
 */

#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/log_entry.hpp"


namespace agent_framework {
namespace model {

/*! LogService */
class LogService : public Resource {
public:

    explicit LogService(const std::string &parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~LogService();


    LogService(const LogService &) = default;


    LogService(LogService &&) = default;


    LogService &operator=(const LogService &) = default;


    LogService &operator=(LogService &&) = default;


    /*!
     * @brief construct an object of class LogService from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed LogService object
     */
    static LogService from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSON
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return LogService::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return LogService::component;
    }


    /*!
     * @brief Set date time
     * @param[in] date_time date time
     * */
    void set_date_time(const OptionalField<std::uint32_t>& date_time) {
        m_date_time = date_time;
    }


    /*!
     * @brief Get date time
     * @return date time
     * */
    const OptionalField<std::uint32_t>& get_date_time() const {
        return m_date_time;
    }


    /*!
     * @brief Set log entry type
     * @param[in] log_entry_type log entry type
     * */
    void set_log_entry_type(const OptionalField<enums::LogEntryType>& log_entry_type) {
        m_log_entry_type = log_entry_type;
    }


    /*!
     * @brief Get log entry type
     * @return log entry type
     * */
    const OptionalField<enums::LogEntryType>& get_log_entry_type() const {
        return m_log_entry_type;
    }


    /*!
     * @brief Set overwrite policy
     * @param[in] overwrite_policy log overwrite policy
     * */
    void set_overwrite_policy(const OptionalField<enums::OverWritePolicy>& overwrite_policy) {
        m_overwrite_policy = overwrite_policy;
    }


    /*!
     * @brief Get override policy
     * @return log overwrite policy
     * */
    const OptionalField<enums::OverWritePolicy>& get_overwrite_policy() const {
        return m_overwrite_policy;
    }


    /*!
     * @brief Set service enabled flag.
     * @param[in] service_enabled service enabled
     * */
    void set_service_enabled(const OptionalField<bool>& service_enabled) {
        m_service_enabled = service_enabled;
    }


    /*!
     * @brief Check if service is enabled.
     * @return true if is enabled, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& is_service_enabled() const {
        return m_service_enabled;
    }


    /*!
     * @brief Set maximum number of records.
     * @param[in] max_number_of_records maximum number of records
     * */
    void set_max_number_of_records(const OptionalField<std::uint32_t>& max_number_of_records) {
        m_max_number_of_records = max_number_of_records;
    }


    /*!
     * @brief Get maximum number of records.
     * @return maximum number of records
     * */
    const OptionalField<std::uint32_t>& get_max_number_of_records() const {
        return m_max_number_of_records;
    }

private:
    OptionalField<std::uint32_t> m_date_time{};
    OptionalField<enums::LogEntryType> m_log_entry_type{};
    OptionalField<enums::OverWritePolicy> m_overwrite_policy{};
    OptionalField<bool> m_service_enabled{};
    OptionalField<std::uint32_t> m_max_number_of_records{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
