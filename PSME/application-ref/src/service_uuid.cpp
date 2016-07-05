/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "service_uuid.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "configuration/configuration.hpp"
#include "configuration/utils.hpp"
#include "agent-framework/logger_ext.hpp"
#include "uuid++.hh"
#include <sys/stat.h>


namespace {
    bool file_exists(const std::string& file_name) {
        struct stat buf;
        return (0 == stat(file_name.c_str(), &buf));
    }

    void generate_uuid_file(const std::string& file_name, const std::string& uuid) {
        json::Value uuid_json;
        uuid_json[app::rest::constants::Common::UUID] = uuid;
        try {
            configuration::json_to_file(file_name, uuid_json);
        } catch (const std::exception& e) {
            log_error(GET_LOGGER("rest"),
                " Error writing Service UUID file: " << file_name);
            throw e;
        }
    }

    std::string get_uuid_filename() {
        static constexpr const char UUID_FILENAME_PROP[] = "service-uuid-file";
        const json::Value& config =
                configuration::Configuration::get_instance().to_json();
        return config[UUID_FILENAME_PROP].as_string();
    }

    std::string try_retrieve_service_uuid(const std::string& uuid_filename,
                                          std::string uuid_string) {
        if (!file_exists(uuid_filename)) {
            log_info(GET_LOGGER("rest"),
                " Generating Service UUID file: " << uuid_filename);
            generate_uuid_file(uuid_filename, uuid_string);
        } else {
            log_info(GET_LOGGER("rest"),
                " Reading Service UUID file: " << uuid_filename);
            json::Value uuid_json;
            configuration::file_to_json(uuid_filename, uuid_json);
            // check if uuid is valid
            uuid u(uuid_json[app::rest::constants::Common::UUID].as_char());
            uuid_string = u.string();
        }
        log_info(GET_LOGGER("rest"), " Service UUID: " << uuid_string);
        return uuid_string;
    }

    std::string retrieve_service_uuid() {
        uuid u;
        u.make(UUID_MAKE_V1);
        std::string service_uuid(u.string());
        try {
            return try_retrieve_service_uuid(get_uuid_filename(), service_uuid);
        } catch (...) {
            log_error(GET_LOGGER("rest"),
                    " Failed to obtain persistent service UUID."
                    " Using temporary one: " << service_uuid);
        }
        return service_uuid;
    }
}

using namespace psme::app;

ServiceUuid::ServiceUuid()
    : m_service_uuid{retrieve_service_uuid()} { }

ServiceUuid::~ServiceUuid() { }

const std::string& ServiceUuid::get_service_uuid() {
    return m_service_uuid;
}
