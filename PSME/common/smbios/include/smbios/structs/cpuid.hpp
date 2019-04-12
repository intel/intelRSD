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
 * @file cpuid.hpp
 * */

#pragma once




#include <cstdint>



namespace smbios {
namespace parser {

constexpr uint8_t REGISTER_WIDTH = 16;

#pragma pack(push, 1)

/*!
 * @brief Processor CPUID
 */
struct CPUID {
    uint8_t socket_designation;
    uint8_t sub_type;
    uint8_t eax_00[REGISTER_WIDTH];
    uint8_t eax_01[REGISTER_WIDTH];
    uint8_t eax_02[REGISTER_WIDTH];
    uint8_t eax_03[REGISTER_WIDTH];
    uint8_t eax_04[REGISTER_WIDTH];
    uint8_t eax_05[REGISTER_WIDTH];
    uint8_t eax_06[REGISTER_WIDTH];
    uint8_t eax_07[REGISTER_WIDTH];
    uint8_t eax_09[REGISTER_WIDTH];
    uint8_t eax_0a[REGISTER_WIDTH];
    uint8_t eax_0b[REGISTER_WIDTH];
    uint8_t eax_0d[REGISTER_WIDTH];
    uint8_t eax_0f[REGISTER_WIDTH];
    uint8_t eax_10[REGISTER_WIDTH];
};


/*!
 * @brief Processor CPUID
 */
struct CPUID_V2 {
    uint8_t socket_designation;
    uint8_t sub_type;
    uint8_t eax_14[REGISTER_WIDTH];
    uint8_t eax_15[REGISTER_WIDTH];
    uint8_t eax_16[REGISTER_WIDTH];
    uint8_t eax_17_00[REGISTER_WIDTH];
    uint8_t eax_17_01[REGISTER_WIDTH];
    uint8_t eax_17_02[REGISTER_WIDTH];
    uint8_t eax_17_03[REGISTER_WIDTH];
    uint8_t eax_80000000[REGISTER_WIDTH];
    uint8_t eax_80000001[REGISTER_WIDTH];
    uint8_t eax_80000002[REGISTER_WIDTH];
    uint8_t eax_80000003[REGISTER_WIDTH];
    uint8_t eax_80000004[REGISTER_WIDTH];
    uint8_t eax_80000006[REGISTER_WIDTH];
    uint8_t eax_80000007[REGISTER_WIDTH];
    uint8_t eax_80000008[REGISTER_WIDTH];
};

/*!
 * @brief SMBIOS CPUID (Type 193)
 */
struct SMBIOS_CPUID_DATA {
    static constexpr uint8_t ID = 193;
    SMBIOS_OOB_HEADER header;
    CPUID data;
};

/*!
 * @brief SMBIOS CPUID (Type 193)
 */
struct SMBIOS_CPUID_DATA_V2 {
    static constexpr uint8_t ID = 193;
    SMBIOS_OOB_HEADER header;
    CPUID_V2 data;
};

#pragma pack(pop)

}
}
