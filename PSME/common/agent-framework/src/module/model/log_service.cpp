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
 * @file log_service.cpp
 */

#include "agent-framework/module/model/log_service.hpp"
#include "agent-framework/module/constants/common.hpp"



using namespace agent_framework::model;

const enums::Component LogService::component = enums::Component::LogService;
const enums::CollectionName LogService::collection_name = enums::CollectionName::LogServices;


LogService::LogService(const std::string& parent_uuid, enums::Component parent_type) : Resource{parent_uuid, parent_type} {
    set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
}


LogService::~LogService() { }


LogService LogService::from_json(const json::Json& json) {
    LogService log_service{};

    log_service.set_date_time(json[literals::LogService::DATETIME]);
    log_service.set_log_entry_type(json[literals::LogService::ENTRY_TYPE]);
    log_service.set_overwrite_policy(json[literals::LogService::OVERWRITE_POLICY]);
    log_service.set_service_enabled(json[literals::LogService::SERVICE_ENABLED]);
    log_service.set_max_number_of_records(json[literals::LogService::MAX_NUMBER_OF_RECORDS]);
    log_service.set_status(attribute::Status::from_json(json[literals::LogService::STATUS]));
    log_service.set_collections(Collections::from_json(json[literals::Collections::COLLECTIONS]));

    return log_service;
}


json::Json LogService::to_json() const {
    json::Json json = json::Json();

    json[literals::LogService::DATETIME] = get_date_time();
    json[literals::LogService::ENTRY_TYPE] = get_log_entry_type();
    json[literals::LogService::OVERWRITE_POLICY] = get_overwrite_policy();
    json[literals::LogService::SERVICE_ENABLED] = is_service_enabled();
    json[literals::LogService::MAX_NUMBER_OF_RECORDS] = get_max_number_of_records();
    json[literals::LogService::STATUS] = get_status().to_json();
    json[literals::Collections::COLLECTIONS] = get_collections().to_json();

    return json;
}
