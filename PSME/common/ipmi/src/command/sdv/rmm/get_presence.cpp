/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file get_presence.cpp
 *
 * @brief Base class for Get[Chassi,Fan,Psu]Presence IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_presence.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetPresence::GetPresence(Cmd cmd) : Request(sdv::NetFn::INTEL, cmd) {}

void request::GetPresence::pack(std::vector<std::uint8_t>& /*data*/) const {}

response::GetPresence::GetPresence(Cmd cmd, const size_t resp_size): Response(sdv::NetFn::INTEL+1, cmd, resp_size) {}

void response::GetPresence::unpack(const std::vector<std::uint8_t>& data) {
    m_presence_mask = data[1];
}

std::uint8_t response::GetPresence::get_presence_mask() const {
    return m_presence_mask;
}
