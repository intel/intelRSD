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

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

class EventData {
public:
    using Vector = std::vector<EventData>;


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


    enums::Notification get_notification() const {
        return m_notification;
    }


    void set_notification(const enums::Notification notification) {
        m_notification = notification;
    }


    const Uuid& get_parent() const {
        return m_parent;
    }


    void set_parent(const Uuid& parent) {
        m_parent = parent;
    }


    enums::Component get_type() const {
        return m_type;
    }


    void set_type(const enums::Component component) {
        m_type = component;
    }


private:
    model::enums::Component m_type{model::enums::Component::None};
    model::enums::Notification m_notification{model::enums::Notification::Add};
    Uuid m_parent{};
    Uuid m_component{};
};

}
}
}
