/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/service_uuid.hpp"

#include <uuid++.hh>



using agent_framework::generic::ServiceUuid;

namespace {
std::string make_uuid() {
    uuid id;
    id.make(UUID_MAKE_V1);
    char* uuid_char = id.string();
    std::string uuid_str{uuid_char};
    free(uuid_char);
    return uuid_str;
}



std::string make_persistent_uuid(const std::string& namespace_uuid, const std::string& unique_name) {
    uuid id{};
    uuid ns_uuid(namespace_uuid.c_str());
    id.make(UUID_MAKE_V5, &ns_uuid, unique_name.c_str());
    char* uuid_char = id.string();
    std::string persistent_id{uuid_char};
    free(uuid_char);
    return persistent_id;
}
}

namespace agent_framework {
namespace model {

const char Resource::STATUS_KEY[] = "status";


Resource::Resource(const std::string& parent_uuid, enums::Component
parent_type) :
    m_temporary_uuid{::make_uuid()},
    m_parent_uuid{parent_uuid},
    m_parent_type{parent_type} {
}


const std::string Resource::make_persistent_uuid() {
    if (m_unique_key.has_value()) {
        std::string namespace_uuid = ServiceUuid::get_instance()->get_service_uuid();
        m_persistent_uuid = ::make_persistent_uuid(namespace_uuid, m_unique_key);
        m_is_uuid_persistent = true;
    }
    return m_persistent_uuid;
}


const std::string Resource::make_random_uuid() {
    m_temporary_uuid = ::make_uuid();
    m_is_uuid_persistent = false;
    return m_temporary_uuid;
}


Resource::~Resource() { }

}
}
