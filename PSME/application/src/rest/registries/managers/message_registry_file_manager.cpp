/*!
 * @brief MessageRegistryFileManager class implementation
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
 * @file message_registry_file_manager.cpp
 * */

#include "psme/rest/registries/managers/message_registry_file_manager.hpp"

#include <algorithm>
#include <exception>



namespace psme {
namespace rest {
namespace registries {

MessageRegistryFileManager::~MessageRegistryFileManager() { }


void MessageRegistryFileManager::add_registry_file(MessageRegistryFile message_registry_file) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};

    increment_max_id();
    message_registry_file.set_id(get_max_id());
    m_id_to_uuid_map.insert({message_registry_file.get_id(), message_registry_file.get_uuid()});
    m_uuid_to_registry_file_map.insert({message_registry_file.get_uuid(), message_registry_file});
}


const MessageRegistryFile& MessageRegistryFileManager::get_file_by_uuid(const std::string& file_uuid) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    return m_uuid_to_registry_file_map.at(file_uuid);
}


const MessageRegistryFile& MessageRegistryFileManager::get_file_by_id(uint64_t file_id) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};

    auto uuid = m_id_to_uuid_map.at(file_id);
    return m_uuid_to_registry_file_map.at(uuid);
}


MessageRegistryFileManager::MessageRegistryFiles MessageRegistryFileManager::get_files() const {
    MessageRegistryFiles files{};

    for (UuidToRegistryFileMap::const_iterator it = m_uuid_to_registry_file_map.begin();
         it != m_uuid_to_registry_file_map.end(); it++) {
        files.emplace_back(it->second);
    }

    return files;
}


std::size_t MessageRegistryFileManager::get_count() const {
    return m_uuid_to_registry_file_map.size();
}


uint64_t MessageRegistryFileManager::get_max_id() const {
    return m_max_id;
}


void MessageRegistryFileManager::increment_max_id() {
    m_max_id += 1u;
}

}
}
}
