/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
    const json::Value& config = configuration::Configuration::get_instance().to_json();
    if (!config.is_member("service")) {
        m_service_uuid = get_temporary_uuid();
        log_info(LOGUSR, "No service defined in config, temporary UUID " << m_service_uuid << " assigned.");
    }
    else {
        database::Database::SPtr db = database::Database::create("service");
        database::String service{config["service"].as_string()};
        database::String uuid{};
        if (db->get(service, uuid)) {
            m_service_uuid = uuid;
            log_info(LOGUSR, "Service " << service << " UUID " << uuid << " read and assigned.");
        }
        else {
            m_service_uuid = get_temporary_uuid();
            uuid = m_service_uuid;
            db->put(service, uuid);
            log_info(LOGUSR, "Service " << service << " UUID " << uuid << " assigned and stored.");
        }
    }
}

ServiceUuid::~ServiceUuid() {}
