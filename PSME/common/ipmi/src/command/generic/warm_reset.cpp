/*!
 * @brief WarmReset command implementation.
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
 * @file warm_reset.cpp
 */


#include "ipmi/command/generic/warm_reset.hpp"
#include "ipmi/command/generic/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::generic;


request::WarmReset::WarmReset() : Request(NetFn::APP, Cmd::WARM_RESET) {}


request::WarmReset::~WarmReset() {}


void request::WarmReset::pack(IpmiInterface::ByteBuffer&) const {}


response::WarmReset::WarmReset() : Response(NetFn::APP + 1, Cmd::WARM_RESET, RESPONSE_SIZE) {}


response::WarmReset::~WarmReset() {}

void response::WarmReset::unpack(const IpmiInterface::ByteBuffer&) { }
