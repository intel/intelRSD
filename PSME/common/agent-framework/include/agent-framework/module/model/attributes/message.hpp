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
 *
 *
 * @file message.hpp
 * @brief Message for Task resource.
 * */

#pragma once

#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "array.hpp"
#include "oem.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace agent_framework {
namespace model {
namespace attribute {

class Message {
public:
    using RelatedProperties = Array<std::string>;
    using MessageArgs = Array<std::string>;

    explicit Message();
    ~Message();

    Message(const OptionalField<std::string>& message_id,
            const OptionalField<std::string>& message_content,
            const OptionalField<enums::Health>& severity,
            const OptionalField<std::string>& resolution,
            const Oem& oem,
            const RelatedProperties& related_properties,
            const MessageArgs& message_args) :
            m_message_id{message_id},
            m_message_content{message_content},
            m_severity{severity},
            m_resolution{resolution},
            m_oem{oem},
            m_related_properties{related_properties},
            m_message_args{message_args}
    {}

    Message(const Message&) = default;
    Message(Message&&) = default;
    Message& operator=(const Message&) = default;
    Message& operator=(Message&&) = default;

    static Message from_json(const json::Json& json);

    json::Json to_json() const;

    const OptionalField<std::string>& get_message_id() const {
        return m_message_id;
    }

    void set_message_id(const OptionalField<std::string>& message_id) {
        m_message_id = message_id;
    }

    const OptionalField<std::string>& get_content() const {
        return m_message_content;
    }

    void set_content(const OptionalField<std::string>& content) {
        m_message_content = content;
    }

    const OptionalField<enums::Health>& get_severity() const {
        return m_severity;
    }

    void set_severity(const OptionalField<enums::Health>& severity) {
        m_severity = severity;
    }

    const OptionalField<std::string>& get_resolution() const {
        return m_resolution;
    }

    void set_resolution(const OptionalField<std::string>& resolution) {
        m_resolution = resolution;
    }

    const Oem& get_oem() const {
        return m_oem;
    }

    void set_oem(const Oem& oem) {
        m_oem = oem;
    }

    const RelatedProperties& get_related_properties() const {
        return m_related_properties;
    }

    void set_related_properties(const RelatedProperties& related_properties) {
        m_related_properties = related_properties;
    }

    const MessageArgs& get_message_args() const {
        return m_message_args;
    }

    void set_message_args(const MessageArgs& message_args) {
        m_message_args = message_args;
    }

private:
    OptionalField<std::string> m_message_id{};
    OptionalField<std::string> m_message_content{};
    OptionalField<enums::Health> m_severity{};
    OptionalField<std::string> m_resolution{};
    Oem m_oem{};
    RelatedProperties m_related_properties{};
    MessageArgs m_message_args{};
};

}
}
}
