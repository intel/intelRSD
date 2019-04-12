/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "psme/rest/server/error/message_object.hpp"
#include "psme/rest/constants/constants.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace psme::rest::error;

namespace {
static const constexpr char TYPE[] = "#Message.v1_0_0.Message";
}


void MessageObject::add_related_property(const std::string& related_property) {
    m_related_properties.push_back(constants::MessageObject::JSON_POINTER_PREFIX + related_property);
}


void MessageObject::set_related_properties(const std::vector<std::string>& related_properties) {
    m_related_properties.clear();
    for (const auto& property : related_properties) {
        m_related_properties.push_back(constants::MessageObject::JSON_POINTER_PREFIX + property);
    }
}


MessageObject::operator json::Json() const {
    json::Json message_object = json::Json();

    message_object[constants::Common::ODATA_TYPE] = ::TYPE;
    message_object[constants::MessageObject::MESSAGE_ID] = get_message_id();
    message_object[constants::MessageObject::MESSAGE] = get_message();
    message_object[constants::MessageObject::RELATED_PROPERTIES] = json::Json::array();
    message_object[constants::MessageObject::MESSAGE_ARGS] = json::Json::array();
    message_object[constants::MessageObject::RESOLUTION] = json::Json();


    if (!get_related_properties().empty()) {
        for (const auto& property : get_related_properties()) {
            message_object[constants::MessageObject::RELATED_PROPERTIES].push_back(property);
        }
    }

    if (!get_message_arguments().empty()) {
        for (const auto& argument : get_message_arguments()) {
            message_object[constants::MessageObject::MESSAGE_ARGS].push_back(argument);
        }
    }

    if (!get_resolution().empty()) {
        message_object[constants::MessageObject::RESOLUTION] = get_resolution();
    }

    message_object[constants::MessageObject::SEVERITY] = get_severity().to_string();
    return message_object;
}

