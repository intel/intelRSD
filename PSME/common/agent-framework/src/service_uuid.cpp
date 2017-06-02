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

#include <uuid++.hh>

extern "C" {
#include <sys/stat.h>
}

using namespace agent_framework::generic;

namespace {
    /*!
    * @brief Check if given file exists
    *
    * Performs basic check before attempting to acces a file. If the provided
    * file path leads to a file which is not a regular file, an exception of
    * type std::runtime_error is thrown.
    *
    * @param[in] file_path Absolute path to file
    *
    * @return True if file exists, false otherwise
    * */
    bool file_exists(const std::string& file_path) {
        struct stat buf;

        if (0 != stat(file_path.c_str(), &buf)) {
            return false;
        }
        if (!S_ISREG(buf.st_mode)) {
            throw std::runtime_error("specified file is not a regular file");
        }

        return true;
    }

    /*!
    * @brief Fetches service UUID from the provided file
    *
    * @param[in] uuid_file_path Absolute path to file containing the UUID
    *
    * @return Persistent service UUID
    * */
    std::string retrieve_service_uuid(const std::string& uuid_file_path) {
        json::Value uuid_json;
        configuration::file_to_json(uuid_file_path, uuid_json);
        // @TODO: check if uuid is valid
        uuid uuid_from_file(
            uuid_json[agent_framework::model::literals::ServiceUuid::UUID]
            .as_char());
        return uuid_from_file.string();
    }

    /*!
    * @brief Store UUID in a file
    *
    * @param[in] file_path Absolute path to file where UUID is to be stored
    * @param[in] service_uuid UUID to be stored in the file
    * */
    void store_service_uuid(const std::string& file_path, const std::string& service_uuid) {
        json::Value uuid_json;
        uuid_json["UUID"] = service_uuid;
        configuration::json_to_file(file_path, uuid_json);
}

}

ServiceUuid::ServiceUuid() {
    uuid temporary_uuid;
    temporary_uuid.make(UUID_MAKE_V1);
    char* uuid_char = temporary_uuid.string();
    m_service_uuid = std::string{uuid_char};
    free(uuid_char);

    try {
        const json::Value& config =
                configuration::Configuration::get_instance().to_json();

        std::string uuid_file_path = config["service-uuid-file"].as_string();
        if (file_exists(uuid_file_path)) {
            m_service_uuid = retrieve_service_uuid(uuid_file_path);
        }
        else {
            log_info(LOGUSR, "Service UUID file not found, generating it");
            store_service_uuid(uuid_file_path, m_service_uuid);
        }
    }
    catch (const std::exception& e) {
         log_error(LOGUSR, "Error occured while handling persistent service UUID file: " <<
                          e.what() << ". Using temporary UUID.");
    }
    catch (...) {
        log_error(LOGUSR, "Unknown error occurred while handling persistent service UUID file, using temporary UUID.");
    }

    log_info(LOGUSR, "Service UUID: " << m_service_uuid);
}

ServiceUuid::~ServiceUuid() {}
