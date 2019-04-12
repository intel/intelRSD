/*!
 * @brief MessageRegistry class definition
 *
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
 * @file message_registry.hpp
 * */


#pragma once



#include "agent-framework/module/model/attributes/message.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/utils/utils.hpp"



namespace psme {
namespace rest {
namespace registries {

/*!
 * Class representing redfish message registry in the data model.
 * */
class MessageRegistry {
public:
    using Messages = agent_framework::model::attribute::Array<agent_framework::model::attribute::Message>;


    explicit MessageRegistry(const OptionalField<std::string>& registry_file);
    MessageRegistry(const MessageRegistry&) = default;
    MessageRegistry(MessageRegistry&&) = default;


    ~MessageRegistry() = default;


    MessageRegistry& operator=(const MessageRegistry&) = default;
    MessageRegistry& operator=(MessageRegistry&&) = default;


    /*!
     * Set message registry language.
     *
     * @param[in] language Message registry language.
     * */
    void set_language(const OptionalField<std::string>& language);


    /*!
     * Get message registry language.
     *
     * @return Message registry language.
     * */
    const OptionalField<std::string>& get_language() const;


    /*!
     * Set message registry prefix.
     *
     * @param[in] registry_prefix Message registry prefix.
     * */
    void set_registry_prefix(const OptionalField<std::string>& registry_prefix);


    /*!
     * Get message registry prefix.
     *
     * @return Message registry prefix.
     * */
    const OptionalField<std::string>& get_registry_prefix() const;


    /*!
     * Set message registry version.
     *
     * @param[in] registry_version Message registry version.
     * */
    void set_registry_version(const OptionalField<std::string>& registry_version);


    /*!
     * Get message registry version.
     *
     * @return Message registry version.
     * */
    const OptionalField<std::string>& get_registry_version() const;


    /*!
     * Set message registry owning entity.
     *
     * @param[in] owning_entity Message registry owning entity.
     * */
    void set_owning_entity(const OptionalField<std::string>& owning_entity);


    /*!
     * Get message registry owning entity.
     *
     * @return Message registry owning entity.
     * */
    const OptionalField<std::string>& get_owning_entity() const;


    /*!
     * Set messages associated with message registry.
     *
     * @param[in] messages Messages associated with message registry.
     * */
    void set_messages(Messages messages);


    /*!
     * Get messages associated with message registry.
     *
     * @return Messages associated with message registry.
     * */
    const Messages& get_messages() const;


private:
    OptionalField<std::string> m_language{};
    OptionalField<std::string> m_registry_prefix{};
    OptionalField<std::string> m_registry_version{};
    OptionalField<std::string> m_owning_entity{};
    Messages m_messages{};

    std::string m_registry_file{};
};

}
}
}

