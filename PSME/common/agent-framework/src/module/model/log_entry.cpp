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
 * @file log_entry.cpp
 */

#include "agent-framework/module/model/log_entry.hpp"
#include "agent-framework/module/constants/common.hpp"



using namespace agent_framework::model;

const enums::Component LogEntry::component = enums::Component::LogEntry;
const enums::CollectionName LogEntry::collection_name = enums::CollectionName::LogEntries;


LogEntry::LogEntry(const std::string& parent_uuid, enums::Component parent_type) : Resource{parent_uuid, parent_type} {
    set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
}


LogEntry::~LogEntry() { }


LogEntry LogEntry::from_json(const json::Json& json) {
    LogEntry log_entry{};

    log_entry.set_created(json[literals::LogEntry::CREATED]);
    log_entry.set_entry_code(json[literals::LogEntry::ENTRY_CODE]);
    log_entry.set_event_type(json[literals::LogEntry::EVENT_TYPE]);
    log_entry.set_entry_type(json[literals::LogEntry::ENTRY_TYPE]);
    log_entry.set_event_id(json[literals::LogEntry::EVENT_ID]);
    log_entry.set_event_timestamp(json[literals::LogEntry::EVENT_TIMESTAMP]);
    log_entry.set_severity(json[literals::LogEntry::SEVERITY]);
    log_entry.set_sensor_type(json[literals::LogEntry::SENSOR_TYPE]);
    log_entry.set_sensor_number(json[literals::LogEntry::SENSOR_NUMBER]);
    log_entry.set_status(attribute::Status::from_json(json[literals::LogService::STATUS]));
    log_entry.set_message(json[literals::Component::MESSAGE]);
    log_entry.set_message_id(json[literals::Message::MESSAGE_ID]);
    log_entry.set_record_id(json[literals::LogEntry::RECORD_ID]);

    return log_entry;
}


json::Json LogEntry::to_json() const {
    json::Json json = json::Json();

    json[literals::LogEntry::CREATED] = get_created();
    json[literals::LogEntry::ENTRY_CODE] = get_entry_code();
    json[literals::LogEntry::ENTRY_TYPE] = get_entry_type();
    json[literals::LogEntry::EVENT_TYPE] = get_event_type();
    json[literals::LogEntry::EVENT_ID] = get_event_id();
    json[literals::LogEntry::EVENT_TIMESTAMP] = get_event_timestamp();
    json[literals::LogEntry::SEVERITY] = get_severity();
    json[literals::LogEntry::SENSOR_TYPE] = get_sensor_type();
    json[literals::LogEntry::SENSOR_NUMBER] = get_sensor_number();
    json[literals::LogEntry::STATUS] = get_status().to_json();
    json[literals::Component::MESSAGE] = get_message();
    json[literals::Message::MESSAGE_ID] = get_message_id();
    json[literals::LogEntry::RECORD_ID] = get_record_id();

    return json;
}
