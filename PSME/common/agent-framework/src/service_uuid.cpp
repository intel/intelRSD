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
 * */

#include "agent-framework/service_uuid.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "configuration/configuration.hpp"
#include "configuration/utils.hpp"
#include "database/database.hpp"

#include <uuid++.hh>

using namespace agent_framework::generic;

namespace {

std::string get_temporary_uuid() {
    uuid temporary_uuid;
    temporary_uuid.make(UUID_MAKE_V1);
    char* uuid_char = temporary_uuid.string();
    std::string service_uuid{uuid_char};
    free(uuid_char);
    return service_uuid;
}

}

ServiceUuid::ServiceUuid() {
    try {
        const json::Value& config = configuration::Configuration::get_instance().to_json();

        database::Database::SPtr db = database::Database::create("service");
        database::String service_file{config["service-uuid-file"].as_string()};
        database::String value{};
        if (db->get(service_file, value)) {
            json::Value json{};
            if (configuration::string_to_json(std::string(value), json)) {
                m_service_uuid = json[agent_framework::model::literals::ServiceUuid::UUID].as_string();
                log_info(LOGUSR, "Service UUID " << m_service_uuid << ".");
            }
            else {
                m_service_uuid = get_temporary_uuid();
                log_error(LOGUSR, "Cannot parse persistent service UUID file. Using temporary UUID " << m_service_uuid << ".");
            }
        }
        else {
            m_service_uuid = get_temporary_uuid();
            json::Value json{};
            json[agent_framework::model::literals::ServiceUuid::UUID] = m_service_uuid;
            value = configuration::json_value_to_string(json);
            db->put(service_file, value);
            log_info(LOGUSR, "Service UUID " << m_service_uuid << " assigned and stored.");
        }
    }
    catch (const std::exception& e) {
        m_service_uuid = get_temporary_uuid();
        log_error(LOGUSR, "Error occured while handling persistent service UUID file: " <<
                          e.what() << ". Using temporary UUID " << m_service_uuid << ".");
    }
    catch (...) {
        m_service_uuid = get_temporary_uuid();
        log_error(LOGUSR, "Unknown error occurred while handling persistent service UUID file, using temporary UUID "
                          << m_service_uuid << ".");
    }
}

ServiceUuid::~ServiceUuid() {}
