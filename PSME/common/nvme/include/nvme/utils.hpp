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
 * @file nvme/utils.hpp
 */

#pragma once

#include "nvme/definitions.hpp"

namespace nvme {

/*!
 * @brief Extracts NVMe version from the version dword
 * @param version Numeric version representation
 * @return Decoded NVMe version
 */
NvmeVersion get_nvme_version(uint32_t version);

/*!
 * @brief Checks if NamespaceAttachment and NamespaceManagement commands are supported
 * @param controller_data Queried controller data
 * @return True if commands are supported
 */
bool supports_namespace_management(const ControllerData& controller_data);

/*!
 * @brief Extracts formatted LBA data size from the namespace data
 * @param namespace_data Queried namespace data
 * @return Formatted LBA data size, if not available, returns default (512)
 */
uint64_t get_formatted_lba_data_size(const NamespaceData& namespace_data);

/*!
 * @brief Gets name of the first namespace from the list.
 * @param ns_id_list List of namespace IDs.
 * @return First namespace identifier.
 * @throws NVMeException if first namespace could not be found.
 */
uint32_t find_first_namespace(const NamespaceIdList& ns_id_list);

}
