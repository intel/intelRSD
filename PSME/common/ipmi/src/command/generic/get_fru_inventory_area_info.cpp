/*!
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
 * @file command/generic/get_fru_inventory_area_info.cpp
 */

#include "ipmi/command/generic/get_fru_inventory_area_info.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <sstream>
#include <iomanip>

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetFruInventoryAreaInfo::GetFruInventoryAreaInfo() : Request(generic::NetFn::STORAGE, generic::Cmd::GET_FRU_INVENTORY_AREA_INFO) {}
request::GetFruInventoryAreaInfo::~GetFruInventoryAreaInfo() {}

void request::GetFruInventoryAreaInfo::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_device_id);
}

response::GetFruInventoryAreaInfo::GetFruInventoryAreaInfo() : Response(generic::NetFn::STORAGE, generic::Cmd::GET_FRU_INVENTORY_AREA_INFO, RESPONSE_SIZE) {}
response::GetFruInventoryAreaInfo::~GetFruInventoryAreaInfo() {}

void response::GetFruInventoryAreaInfo::unpack(const IpmiInterface::ByteBuffer& data) {
    m_size = uint16_t(data[DATA_OFFSET_SIZE_LSB] + (data[DATA_OFFSET_SIZE_MSB] << 8));
    m_access_mode = (data[DATA_OFFSET_ACCESS_MODE] & DATA_MASK_ACCESS_MODE) == 0 ? FruAccessMode::BYTE : FruAccessMode::WORD;
}
