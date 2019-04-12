/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
*/

#include "psme/core/agent/agent_unreachable.hpp"



using namespace psme::core::agent;


AgentUnreachable::AgentUnreachable(const std::string& gami_id)
    : m_gami_id(gami_id) {
    make_what();
}


AgentUnreachable::~AgentUnreachable() noexcept {}


const std::string& AgentUnreachable::get_gami_id() const {
    return m_gami_id;
}


const char* AgentUnreachable::what() const noexcept {
    return m_what.c_str();
}


void AgentUnreachable::make_what() {
    m_what = std::string("Could not connect to agent [ID = '" + get_gami_id() + "']");
}