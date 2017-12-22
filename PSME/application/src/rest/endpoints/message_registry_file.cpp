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
 * */

#include <psme/rest/server/error/server_exception.hpp>
#include <psme/rest/server/error/error_factory.hpp>
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/message_registry_file.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/registries/managers/message_registry_file_manager.hpp"

#include "agent-framework/exceptions/not_found.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::endpoint::utils;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[constants::Common::ODATA_CONTEXT] =
        "/redfish/v1/$metadata#MessageRegistryFile.MessageRegistryFile";
    r[constants::Common::ODATA_ID] = json::Value::Type::NIL;
    r[constants::Common::ODATA_TYPE] = "#MessageRegistryFile.v1_0_0.MessageRegistryFile";
    r[constants::Common::ID] = json::Value::Type::NIL;
    r[constants::Common::NAME] = json::Value::Type::NIL;
    r[constants::Common::DESCRIPTION] = json::Value::Type::NIL;

    r[constants::MessageRegistryFile::LANGUAGES] = json::Value::Type::ARRAY;
    r[constants::MessageRegistryFile::REGISTRY] = json::Value::Type::NIL;
    r[constants::MessageRegistryFile::LOCATION] = json::Value::Type::ARRAY;
    r[constants::Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}


MessageRegistryFile::MessageRegistryFile(const std::string& path) : EndpointBase(path) { }


MessageRegistryFile::~MessageRegistryFile() { }


void MessageRegistryFile::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[constants::Common::ODATA_ID] = request.get_url();
    r[constants::Common::ID] = request.params[constants::PathParam::MESSAGE_REGISTRY_FILE_ID];

    const auto& file_id = id_to_uint64(request.params[constants::PathParam::MESSAGE_REGISTRY_FILE_ID]);

    try {
        const auto& file = registries::MessageRegistryFileManager::get_instance()->get_file_by_id(file_id);

        r[constants::Common::NAME] = file.get_name();
        r[constants::Common::DESCRIPTION] = file.get_description();

        json::Value languages = json::Value::Type::ARRAY;
        for (const auto& language : file.get_languages()) {
            languages.push_back(language);
        }
        r[constants::MessageRegistryFile::LANGUAGES] = std::move(languages);
        r[constants::MessageRegistryFile::REGISTRY] = file.get_registry();

        json::Value locations = json::Value::Type::ARRAY;
        for (const auto& location : file.get_locations()) {
            json::Value s = json::Value::Type::OBJECT;

            // All properties of file's location are not nullable in metadata, but they are not required, too.
            // Therefore, if they have no values, they will not be displayed.
            if (location.get_language().has_value()) {
                s[constants::MessageRegistryFile::LANGUAGE] = location.get_language();
            }
            if (location.get_uri().has_value()) {
                s[constants::MessageRegistryFile::URI] = location.get_uri();
            }
            if (location.get_archive_uri().has_value()) {
                s[constants::MessageRegistryFile::ARCHIVE_URI] = location.get_archive_uri();
            }
            if (location.get_publication_uri().has_value()) {
                s[constants::MessageRegistryFile::PUBLICATION_URI] = location.get_publication_uri();
            }
            if (location.get_archive_file().has_value()) {
                s[constants::MessageRegistryFile::ARCHIVE_FILE] = location.get_archive_file();
            }
            locations.push_back(std::move(s));
        }
        r[constants::MessageRegistryFile::LOCATION] = std::move(locations);

    }
    catch (const std::out_of_range&) {
        throw agent_framework::exceptions::NotFound("Requested message registry file does not exist.");
    }

    set_response(response, r);
}
