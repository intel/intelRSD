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
 * @file nvme/utils.cpp
 */

#include "nvme/utils.hpp"
#include "nvme/nvme_exception.hpp"

namespace {

static constexpr uint16_t FLAG_OACS_SUPPORTS_NAMESPACE_MANAGEMENT = 0x0008;
static constexpr uint8_t MASK_FLBAS_LBA_FORMAT = 0x0F;
static constexpr uint32_t MASK_VERSION_PRE_1_2 = 0xFFFFFF00;

static constexpr uint64_t DEFAULT_BLOCK_SIZE_BYTES = 512;

}

nvme::NvmeVersion nvme::get_nvme_version(uint32_t version) {
    using namespace nvme;

    if (version == 0) {
        return NvmeVersion::UNKNOWN;
    }

    uint32_t masked_version = version & MASK_VERSION_PRE_1_2;

    switch (NvmeVersion(masked_version)) {
    case NvmeVersion::NVME_1_0_0:
        return NvmeVersion::NVME_1_0_0;
    case NvmeVersion::NVME_1_1_0:
        return NvmeVersion::NVME_1_1_0;

    /* only to avoid gcc warnings */
    case NvmeVersion::NVME_1_2_0:
    case NvmeVersion::NVME_1_2_1:
    case NvmeVersion::NVME_1_3_0:
    case NvmeVersion::INVALID:
    case NvmeVersion::UNKNOWN:
    default:
        break;
    }

    switch (NvmeVersion(version)) {
    case NvmeVersion::NVME_1_2_0:
        return NvmeVersion::NVME_1_2_0;
    case NvmeVersion::NVME_1_2_1:
        return NvmeVersion::NVME_1_2_1;
    case NvmeVersion::NVME_1_3_0:
        return NvmeVersion::NVME_1_3_0;

    /* only to avoid gcc warnings */
    case NvmeVersion::NVME_1_0_0:
    case NvmeVersion::NVME_1_1_0:
    case NvmeVersion::INVALID:
    case NvmeVersion::UNKNOWN:
    default:
        break;
    }
    return NvmeVersion::INVALID;
}

bool nvme::supports_namespace_management(const nvme::ControllerData& controller_data) {
    return (controller_data.oacs & FLAG_OACS_SUPPORTS_NAMESPACE_MANAGEMENT) > 0;
}

uint64_t nvme::get_formatted_lba_data_size(const nvme::NamespaceData& namespace_data) {
    uint8_t flbas_id = namespace_data.flbas & MASK_FLBAS_LBA_FORMAT;
    if (flbas_id >= LBA_FORMAT_COUNT) {
        /* Invalid value, return zero to indicate it */
        return DEFAULT_BLOCK_SIZE_BYTES;
    }
    uint8_t flbas_power = namespace_data.lbaf[flbas_id].lba_data_size;
    if (flbas_power == 0) {
        /* Not supported */
        return DEFAULT_BLOCK_SIZE_BYTES;
    }
    return (1 << flbas_power);
}

std::uint32_t nvme::find_first_namespace(const nvme::NamespaceIdList& ns_id_list) {
    for (unsigned i = 0; i < nvme::NAMESPACE_ID_LIST_MAX_SIZE; ++i) {
        if (ns_id_list.namespace_id[i] != 0) {
            return ns_id_list.namespace_id[i];
        }
    }
    throw nvme::NvmeException("No active namespaces found.");
}