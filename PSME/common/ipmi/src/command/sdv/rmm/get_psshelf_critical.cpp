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
 * @file get_psshelf_critical.cpp
 *
 * @brief GetPsShelfCritical IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_psshelf_critical.hpp"
#include "ipmi/command/sdv/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetPsShelfCritical::GetPsShelfCritical() : Request(NetFn::INTEL, Cmd::GET_PSSHELF_CRITICAL) {}


request::GetPsShelfCritical::~GetPsShelfCritical() {}


response::GetPsShelfCritical::GetPsShelfCritical() : Response(NetFn::INTEL + 1, Cmd::GET_PSSHELF_CRITICAL,
                                                              RESPONSE_SIZE) {}


response::GetPsShelfCritical::~GetPsShelfCritical() {}


void response::GetPsShelfCritical::unpack(const IpmiInterface::ByteBuffer& data) {
    m_is_critical = data[OFFSET_DATA];
}


bool response::GetPsShelfCritical::is_critical() const {
    return m_is_critical;
}
