/*!
 * @brief iSCSI MDR version structure builder.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_mdr_version_builder.cpp
 */

#include "iscsi/iscsi_mdr_version_builder.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

namespace iscsi {
namespace builder {

void IscsiVersionBuilder::append_structure(ipmi::IpmiInterface::ByteBuffer& buffer, std::function<uint16_t()>) {

    push_uint8(buffer, static_cast<uint8_t>(iscsi::structs::MdrStructType::VERSION));
    push_uint16(buffer, static_cast<uint16_t>(sizeof(iscsi::structs::ISCSI_MDR_VERSION)));
    push_uint16(buffer, static_cast<uint16_t>(0));

    push_uint8(buffer, static_cast<uint8_t>(1));
    push_uint8(buffer, static_cast<uint8_t>(0));

    // end the structure
    append_strings(buffer, StringContainer());
}

}  // namespace builder
}  // namespace iscsi
