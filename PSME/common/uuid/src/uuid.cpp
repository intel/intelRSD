/*!
 * @brief definition of wrapper functions for OSSP UUID library
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file common/uuid/src/uuid.cpp
 */

#include "uuid/uuid.hpp"
#include "uuid++.hh"


std::string make_v1_uuid() {
    uuid id;
    id.make(UUID_MAKE_V1);
    char* uuid_buffer = id.string();
    std::string uuid{uuid_buffer};
    free(uuid_buffer);
    return uuid;
}

std::string make_v5_uuid(const std::string& namespace_uuid, const std::string& unique_name) {
    uuid id{};
    uuid ns_uuid(namespace_uuid.c_str());
    id.make(UUID_MAKE_V5, &ns_uuid, unique_name.c_str());
    char* uuid_buffer = id.string();
    std::string uuid{uuid_buffer};
    free(uuid_buffer);
    return uuid;
}


std::string unparse_uuid(const void* guid_ptr) {
    uuid uid(guid_ptr);
    char* ptr = uid.string();
    std::string uuid_str(ptr);
    free(ptr);
    return uuid_str;
}


bool is_valid_string_uuid(const std::string& uuid) {
    return static_cast<bool>(uuid_isstr(uuid.data(), uuid.size()));
}