/*!
* @copyright
* Copyright (c) 2019 Intel Corporation
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

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/manager/log_entry.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "agent-framework/module/common_components.hpp"

#include "logger/logger.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using agent_framework::module::CommonComponents;
using agent_framework::module::get_manager;


namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#LogEntry.LogEntry";
    r[Common::ODATA_TYPE] = "#LogEntry.v1_4_1.LogEntry";
    r[Common::NAME] = "Log Service Entry";
    r[Common::DESCRIPTION] = "Log Service Entry description";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ID] = json::Json::value_t::null;
    r[LogEntry::ENTRY_TYPE] = json::Json::value_t::null;
    r[LogEntry::ENTRY_CODE] = json::Json::value_t::null;
    r[LogEntry::EVENT_TIMESTAMP] = json::Json::value_t::null;
    r[LogEntry::LINKS][LogEntry::ORIGIN_OF_CONDITION] = json::Json::value_t::null;
    r[LogEntry::SENSOR_TYPE] = json::Json::value_t::null;
    r[LogEntry::SENSOR_NUMBER] = json::Json::value_t::null;
    r[LogEntry::MESSAGE_ID] = json::Json::value_t::null;
    r[Common::LINKS] = json::Json::value_t::object;

    return r;
}
}

json::Json endpoint::LogEntry::get_log_entry_json(agent_framework::model::LogEntry log_entry) {

    using namespace agent_framework::model::enums;
    auto r = make_prototype();

    r[Common::ID] = log_entry.get_id();

    r[constants::LogEntry::ENTRY_TYPE] = log_entry.get_entry_type();
    r[constants::LogEntry::EVENT_TIMESTAMP] = log_entry.get_event_timestamp();
    r[constants::LogEntry::SENSOR_TYPE] = log_entry.get_sensor_type();
    r[constants::LogEntry::SENSOR_NUMBER] = log_entry.get_sensor_number();
    r[constants::LogEntry::MESSAGE_ID] = log_entry.get_message_id();

    r[constants::LogEntry::ENTRY_CODE] = log_entry.get_entry_code();

    json::Json rs = json::Json();
    rs[Common::ODATA_TYPE] = "#Intel.Oem.LogEntry";
    rs[constants::LogEntry::RECORD_ID] = log_entry.get_record_id();
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    if (log_entry.get_origin_of_condition().has_value()) {
        if (get_manager<agent_framework::model::System>().entry_exists(log_entry.get_origin_of_condition())) {
            const auto system_uuid = get_manager<agent_framework::model::System>().get_entry(log_entry.get_origin_of_condition()).get_uuid();
            r[Common::LINKS][constants::LogEntry::ORIGIN_OF_CONDITION][Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(Component::System, system_uuid);
        } else if (get_manager<agent_framework::model::Manager>().entry_exists(log_entry.get_origin_of_condition())) {
            const auto manager_uuid = get_manager<agent_framework::model::Manager>().get_entry(log_entry.get_origin_of_condition()).get_uuid();
            r[Common::LINKS][constants::LogEntry::ORIGIN_OF_CONDITION][Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(Component::Manager, manager_uuid);
        } else {
            log_warning("rest", "No implemented manager case for log entry with ID=" << log_entry.get_id());
        }
    }

    endpoint::utils::fill_name_and_description(log_entry, r);

    return r;
}

endpoint::LogEntry::LogEntry(const std::string& path) : EndpointBase(path) {}

endpoint::LogEntry::~LogEntry() {}

void endpoint::LogEntry::get(const server::Request& request, server::Response& response) {

    auto log_entry = psme::rest::model::find<agent_framework::model::Manager, agent_framework::model::LogService, agent_framework::model::LogEntry>(request.params).get();
    auto r = get_log_entry_json(log_entry);

    r[Common::ODATA_ID] = PathBuilder(request).build();

    set_response(response, r);
}

