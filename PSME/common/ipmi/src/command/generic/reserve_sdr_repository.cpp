/*!
 * @brief Implementation of Reserve SDR Repository Request/Response.
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
 * @file command/generic/reserve_sdr_repository.cpp
 */

#include "ipmi/command/generic/reserve_sdr_repository.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::ReserveSdrRepository::ReserveSdrRepository() :
        Request(generic::NetFn::STORAGE, generic::Cmd::RESERVE_SDR_REPOSITORY) {}

request::ReserveSdrRepository::~ReserveSdrRepository() {}

response::ReserveSdrRepository::ReserveSdrRepository() :
        Response(generic::NetFn::STORAGE + 1, generic::Cmd::RESERVE_SDR_REPOSITORY, RESPONSE_SIZE) {}

response::ReserveSdrRepository::~ReserveSdrRepository() {}

void response::ReserveSdrRepository::unpack(const IpmiInterface::ByteBuffer& data) {
    m_reservation_id = std::uint16_t(data[OFFSET_DATA] | data[OFFSET_DATA + 1] << 8);
}
