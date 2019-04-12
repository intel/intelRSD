/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file module/requests/psme/component_notification.cpp
 *
 * @brief PSME component_notification request
 * */

#include "agent-framework/module/requests/psme/component_notification.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::requests;

ComponentNotification::ComponentNotification() {}

json::Json ComponentNotification::to_json() const {
    json::Json value = json::Json();
    value[literals::ComponentNotification::GAMI_ID] = m_gami_id;
    value[literals::ComponentNotification::NOTIFICATIONS] = json::Json::array();
    for (const auto& notification : m_notifications) {
        value[literals::ComponentNotification::NOTIFICATIONS].emplace_back(notification.to_json());
    }
    return value;
}

ComponentNotification ComponentNotification::from_json(const json::Json& value) {
    ComponentNotification component_notification{};
    component_notification.set_gami_id(value[literals::ComponentNotification::GAMI_ID]);

    attribute::EventData::Vector notifications;
    for (auto& notification_json : value.at(literals::ComponentNotification::NOTIFICATIONS)) {
        notifications.emplace_back(attribute::EventData::from_json(notification_json));
    }
    component_notification.set_notifications(notifications);

    return component_notification;
}
