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
 *
 * @file metadata_manager.cpp
 * @brief MetadataManager model implementation
 * */
#include "psme/rest/metadata/metadata_manager.hpp"
#include "psme/rest/metadata/include-generated/initialize_map.hpp"
#include "agent-framework/exceptions/exception.hpp"

using namespace psme::rest::metadata;

const std::string& MetadataManager::get_xml(const std::string& key) {
    auto it = xml_map.find(key);
    if(xml_map.end() == it) {
        THROW(agent_framework::exceptions::NotFound, "rest", "Metadata "
            "file: \"" + key + "\" not found");
    }
    return it->second;
}
