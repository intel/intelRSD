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

#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/module/constants/psme.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace agent_framework::model::attribute;

json::Json EventData::to_json() const {
    json::Json json{};
    json[literals::ComponentNotification::COMPONENT] = get_component();
    json[literals::ComponentNotification::NOTIFICATION] = get_notification().to_string();
    json[literals::ComponentNotification::TYPE] = get_type().to_string();
    json[literals::ComponentNotification::PARENT] = get_parent();
    return json;
}

EventData EventData::from_json(const json::Json& json) {
    EventData data{};
    data.set_component(json[literals::ComponentNotification::COMPONENT]);
    data.set_notification(enums::Notification::from_string(json[literals::ComponentNotification::NOTIFICATION].get<std::string>()));
    data.set_type(enums::Component::from_string(json[literals::ComponentNotification::TYPE].get<std::string>()));
    data.set_parent(json[literals::ComponentNotification::PARENT]);
    return data;
}
