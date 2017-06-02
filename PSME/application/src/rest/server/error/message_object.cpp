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

#include "psme/rest/server/error/message_object.hpp"
#include "psme/rest/constants/constants.hpp"

#include <json/json.hpp>



using namespace psme::rest::error;

namespace {
static const constexpr char TYPE[] = "/redfish/v1/$metadata#Message.v1_0_0.Message";
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


MessageObject::operator json::Value() const {
    json::Value message_object{};

    message_object[constants::Common::ODATA_TYPE] = ::TYPE;
    message_object[constants::MessageObject::MESSAGE_ID] = get_message_id();
    message_object[constants::MessageObject::MESSAGE] = get_message();

    if (!get_related_properties().empty()) {
        json::Value related_properties{};
        for (const auto& property : get_related_properties()) {
            related_properties.push_back(property);
        }
        message_object[constants::MessageObject::RELATED_PROPERTIES] = std::move(related_properties);
    }

    if (!get_message_arguments().empty()) {
        json::Value message_args{};
        for (const auto& argument : get_message_arguments()) {
            message_args.push_back(argument);
        }
        message_object[constants::MessageObject::MESSAGE_ARGS] = std::move(message_args);
    }

    if (!get_resolution().empty()) {
        message_object[constants::MessageObject::RESOLUTION] = get_resolution();
    }

    message_object[constants::MessageObject::SEVERITY] = get_severity().to_string();
    return message_object;
}

