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
 * @file tpm.hpp
 *
 * @brief smbios structures describing tpm components
 * */

#pragma once



#include <cstdint>



namespace smbios {
namespace parser {

#pragma pack(push, 1)

enum class TPMStatus : uint8_t {
    DISABLED = 0x00,
    ENABLED = 0x01
};

struct TPM_INFO {
    uint8_t tpm_configuration_index;
    uint8_t tpm_version;
    TPMStatus tpm_status;
};

/*!
 * @brief SMBIOS TPM Device (Type 195)
 */
struct SMBIOS_TPM_INFO_DATA {
    static constexpr uint8_t ID = 195;
    SMBIOS_OOB_HEADER header;
    TPM_INFO data;
};

#pragma pack(pop)

}
}
