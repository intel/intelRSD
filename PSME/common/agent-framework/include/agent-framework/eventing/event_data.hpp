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

#pragma once
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include <string>

namespace agent_framework {
namespace eventing {

ENUM(Notification, std::uint32_t,
    Add,
    Remove,
    Update
);

class EventData {
public:
    const std::string& get_gami_id() const {
        return m_gami_id;
    }

    void set_gami_id(const std::string& gami_id) {
        m_gami_id = gami_id;
    }

    const std::string& get_component() const {
        return m_component;
    }

    void set_component(const std::string& component) {
        m_component = component;
    }

    Notification get_notification() const {
        return m_notification;
    }

    void set_notification(const Notification notification) {
        m_notification = notification;
    }

    void set_notification(const std::string& notificaton) {
        m_notification = Notification::from_string(notificaton);
    }

    const std::string& get_parent() const {
        return m_parent;
    }

    void set_parent(const std::string& parent) {
        m_parent = parent;
    }

    ::agent_framework::model::enums::Component get_type() const {
        return m_type;
    }

    void set_type(const ::agent_framework::model::enums::Component component) {
        m_type = component;
    }

    void set_type(const std::string& type) {
        m_type = ::agent_framework::model::enums::Component::from_string(type);
    }

    Json::Value to_json() const {
        Json::Value json;

        json["gamiId"] = get_gami_id();
        json["parent"] = get_parent();
        json["type"] = get_type().to_string();
        json["component"] = get_component();
        json["notification"] = get_notification().to_string();

        return json;
    }

private:
    ::agent_framework::model::enums::Component m_type{
            ::agent_framework::model::enums::Component::Fan};
    Notification m_notification{Notification::Add};
    std::string m_parent{};
    std::string m_component{};
    std::string m_gami_id{};
};

}
}

