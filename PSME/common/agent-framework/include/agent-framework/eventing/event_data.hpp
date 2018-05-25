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
 * */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include <string>



namespace agent_framework {
namespace eventing {

class EventData {
public:

    /*!
     * Construct an EventData object from json::Json
     *
     * @param[in] json Json object representing an EventData
     *
     * @return EventData object
     * */
    static EventData from_json(const json::Json& json);


    /*!
     * Convert an EventData object to json::Json
     *
     * @return json::Json object representing an EventData
     * */
    json::Json to_json() const;


    const Uuid& get_component() const {
        return m_component;
    }


    void set_component(const Uuid& component) {
        m_component = component;
    }


    model::enums::Notification get_notification() const {
        return m_notification;
    }


    void set_notification(const model::enums::Notification notification) {
        m_notification = notification;
    }


    void set_notification(const std::string& notification) {
        m_notification = model::enums::Notification::from_string(notification);
    }


    const Uuid& get_parent() const {
        return m_parent;
    }


    void set_parent(const Uuid& parent) {
        m_parent = parent;
    }


    model::enums::Component get_type() const {
        return m_type;
    }


    void set_type(const model::enums::Component component) {
        m_type = component;
    }


    void set_type(const std::string& type) {
        m_type = model::enums::Component::from_string(type);
    }


private:
    model::enums::Component m_type{model::enums::Component::None};
    model::enums::Notification m_notification{model::enums::Notification::Add};
    Uuid m_parent{};
    Uuid m_component{};
};

using EventDataVec = std::vector<EventData>;

/* For backward compatibility, could be removed */
using Notification = agent_framework::model::enums::Notification;

}
}
