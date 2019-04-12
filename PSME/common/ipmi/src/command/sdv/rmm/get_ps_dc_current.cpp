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
 * @file get_ps_dc_current.cpp
 *
 * @brief GetPsDcCurrent IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_ps_dc_current.hpp"
#include "ipmi/command/sdv/enums.hpp"

#include "ipmi/utils/sdv/linear_to_double.hpp"

using namespace ipmi::sdv;
using namespace ipmi::command::sdv;

request::GetPsDcCurrent::GetPsDcCurrent() : Request(NetFn::INTEL, Cmd::GET_PS_DC_CURRENT) {}

void request::GetPsDcCurrent::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_psu_id);
}

response::GetPsDcCurrent::GetPsDcCurrent(): Response(NetFn::INTEL + 1, Cmd::GET_PS_DC_CURRENT, RESPONSE_SIZE) {}

void response::GetPsDcCurrent::unpack(const IpmiInterface::ByteBuffer& data) {
    m_current = linear_to_double(std::uint16_t(data[2] << 8 | data[1]));
}
