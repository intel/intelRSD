/*!
 * @brief MDR region iSCSI Boot Options Version structure definition.
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
 * @file iscsi_mdr_version.hpp
 */

#pragma once

#include "iscsi/structs/iscsi_mdr_header.hpp"

namespace iscsi {
namespace structs {

#pragma pack(push, 1)

struct ISCSI_MDR_VERSION_DATA {
    uint8_t major_version;
    uint8_t minor_version;
};

struct ISCSI_MDR_VERSION {
    static constexpr MdrStructType ID = MdrStructType::VERSION;
    ISCSI_MDR_HEADER header;
    ISCSI_MDR_VERSION_DATA data;
};

#pragma pack(pop)

}  // namespace structs
}  // namespace iscsi
