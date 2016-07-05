/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file command/collection.cpp
 *
 * @brief Collection specific data
 * */

#include "agent-framework/module/collection.hpp"
#include <json/json.h>

using namespace agent_framework::generic;

Collection::~Collection() { }

Json::Value Collection::to_json() const {
    Json::Value collection(Json::arrayValue);
    for (const auto& entry : m_collection) {
        Json::Value json_entry;
        json_entry["name"] = entry.get_name();
        json_entry["type"] = entry.get_type();
        json_entry["slotMask"] = entry.get_slot_mask();

        collection.append(std::move(json_entry));
    }

    return collection;
}

void Collection::CollectionEntry::set_name(const std::string& name) {
    m_name = name;
}

const std::string& Collection::CollectionEntry::get_name() const {
    return m_name;
}

void Collection::CollectionEntry::set_type(const std::string& type) {
    m_type = type;
}

const std::string& Collection::CollectionEntry::get_type() const {
    return m_type;
}

void Collection::CollectionEntry::set_slot_mask(const std::string& slot_mask) {
    m_slot_mask = slot_mask;
}

const std::string& Collection::CollectionEntry::get_slot_mask() const {
    return m_slot_mask;
}

void Collection::set_collection(const Collection::collection_t& collection) {
    m_collection = collection;
}

const Collection::collection_t& Collection::get_collection() const {
    return m_collection;
}

void Collection::add_collection_entry(
                                    const Collection::CollectionEntry& entry) {
    m_collection.emplace_back(entry);
}

