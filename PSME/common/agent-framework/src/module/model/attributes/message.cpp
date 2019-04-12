/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "agent-framework/module/model/attributes/message.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

Message::Message() { }

Message::~Message() { }

json::Json Message::to_json() const {
    json::Json json = json::Json();

    json[literals::Message::MESSAGE_ID] = m_message_id;
    json[literals::Message::MESSAGE_CONTENT] = m_message_content;
    json[literals::Message::RELATED_PROPERTIES] = m_related_properties.to_json();
    json[literals::Message::MESSAGE_ARGS] = m_message_args.to_json();
    json[literals::Message::SEVERITY] = m_severity;
    json[literals::Message::RESOLUTION] = m_resolution;
    json[literals::Message::OEM] = m_oem.to_json();

    return json;
}


Message Message::from_json(const json::Json& json) {
    Message message{};

    message.set_message_id(json[literals::Message::MESSAGE_ID]);
    message.set_content(json[literals::Message::MESSAGE_CONTENT]);
    message.set_severity(json[literals::Message::SEVERITY]);
    message.set_resolution(json[literals::Message::RESOLUTION]);
    message.set_oem(Oem::from_json(json.value(literals::Message::OEM, json::Json())));
    message.set_related_properties(RelatedProperties::from_json(json[literals::Message::RELATED_PROPERTIES]));
    message.set_message_args(MessageArgs::from_json(json[literals::Message::MESSAGE_ARGS]));

    return message;
}
