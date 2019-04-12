/*!
 * @brief ACPI Table header structure
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file acpi_header.hpp
 */

#pragma once

#include <cstdint>

namespace acpi {
namespace structs {

constexpr const char* NFIT = "NFIT";
constexpr const char* PCAT = "PCAT";
constexpr const char* SRAT = "SRAT";
constexpr const char* HMAT = "HMAT";
constexpr const char* PMTT = "PMTT";

#pragma pack(push, 1)

struct ACPI_TABLE_HEADER {
    uint8_t  Signature[4];
    uint32_t Length;
    uint8_t  Revision;
    uint8_t  Checksum;
    uint8_t  OemID[6];
    uint8_t  OemTableID[8];
    uint32_t OemRevision;
    uint32_t CreatorId;
    uint32_t CreatorRevision;
    uint8_t  Rsvd0[4];
};

#pragma pack(pop)

}
}