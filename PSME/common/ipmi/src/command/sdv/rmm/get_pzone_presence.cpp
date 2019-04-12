/*!
 * @brief GetPzonePresence IPMI command request and response implementation.
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
 * @file get_pzone_presence.
 */

#include "ipmi/command/sdv/rmm/get_pzone_presence.hpp"
#include "ipmi/command/sdv/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetPzonePresence::GetPzonePresence() : GetPresence(sdv::Cmd::GET_PZONE_PRESENCE) {}


request::GetPzonePresence::~GetPzonePresence() {}


response::GetPzonePresence::GetPzonePresence() : GetPresence(sdv::Cmd::GET_PZONE_PRESENCE) {}


response::GetPzonePresence::~GetPzonePresence() {}


bool response::GetPzonePresence::is_present(std::uint8_t pzone_index) const {
    return (pzone_index >= get_slot_count()) ? false : (m_presence_mask & (1 << pzone_index));
}
