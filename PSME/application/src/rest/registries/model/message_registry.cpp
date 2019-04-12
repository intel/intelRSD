/*!
 * @brief MessageRegistry class implementation
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
 * @file message_registry.cpp
 * */


#include "psme/rest/registries/model/message_registry.hpp"



namespace psme {
namespace rest {
namespace registries {

MessageRegistry::MessageRegistry(const OptionalField<std::string>& registry_file) {
    m_registry_file = registry_file;
}


void MessageRegistry::set_language(const OptionalField<std::string>& language) {
    m_language = language;
}


const OptionalField<std::string>& MessageRegistry::get_language() const {
    return m_language;
}


void MessageRegistry::set_registry_prefix(const OptionalField<std::string>& registry_prefix) {
    m_registry_prefix = registry_prefix;
}


const OptionalField<std::string>& MessageRegistry::get_registry_prefix() const {
    return m_registry_prefix;
}


void MessageRegistry::set_registry_version(const OptionalField<std::string>& registry_version) {
    m_registry_version = registry_version;
}


const OptionalField<std::string>& MessageRegistry::get_registry_version() const {
    return m_registry_version;
}


void MessageRegistry::set_owning_entity(const OptionalField<std::string>& owning_entity) {
    m_owning_entity = owning_entity;
}


const OptionalField<std::string>& MessageRegistry::get_owning_entity() const {
    return m_owning_entity;
}


void MessageRegistry::set_messages(Messages messages) {
    m_messages = messages;
}


const MessageRegistry::Messages& MessageRegistry::get_messages() const {
    return m_messages;
}

}
}
}

