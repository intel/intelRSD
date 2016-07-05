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
 * */

#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "uuid++.hh"

#include <cstring>
#include <vector>
#include <utility>
#include <cstdlib>


namespace {
    std::string make_uuid() {
        uuid id;
        id.make(UUID_MAKE_V1);
        return id.string();
    }
}

namespace agent_framework {
namespace model {

Resource::Resource(const std::string& parent_uuid, enums::Component
parent_type) :
        m_uuid{::make_uuid()},
        m_parent_uuid{parent_uuid},
        m_parent_type{parent_type} {
}

Resource::~Resource() {}

}
}
