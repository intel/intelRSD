/*!
 * @brief Reset command implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file reset.cpp
 */

#include "ipmi/command/sdv/rmm/reset.hpp"
#include "ipmi/command/sdv/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::Reset::Reset() : Request(NetFn::INTEL, Cmd::RESET),
        m_component_to_reset(static_cast<std::uint8_t>(request::Reset::ComponentToReset::MMP)) { }

request::Reset::Reset(ComponentToReset component_to_reset) : Request(NetFn::INTEL, Cmd::RESET),
        m_component_to_reset(static_cast<std::uint8_t>(component_to_reset)) { }

request::Reset::Reset(std::uint8_t component_to_reset) : Request(NetFn::INTEL, Cmd::RESET),
        m_component_to_reset(component_to_reset) { }

request::Reset::~Reset() { }


void request::Reset::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_component_to_reset);
}


response::Reset::Reset() : Response(NetFn::INTEL + 1, Cmd::RESET, RESPONSE_SIZE) {}


response::Reset::~Reset() {}


void response::Reset::unpack(const IpmiInterface::ByteBuffer&) {}
