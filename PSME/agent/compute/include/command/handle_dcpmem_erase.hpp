/*!
 * @brief Declaration of function for handling Intel Optane DC Persistent Memory erase operation
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file handle_dcpmem_erase.hpp
 */

#pragma once

#include "agent-framework/command/compute_commands.hpp"


namespace agent {
namespace compute {

struct EraseDcpmemAttributes {
    bool reset_configuration;
    bool erase_configuration_keys;
};

/*!
 * @brief Implementation of SetComponentAttributes for Intel Optane DC Persistent Memory Module
 * @param[in] system_uuid the uuid of the System
 * @param[in] erase_dcpmem_attributes the erase attributes to be set
 * @return Created task UUID
 * */
Uuid process_dcpmem_erase(const Uuid& system_uuid, const EraseDcpmemAttributes& erase_dcpmem_attributes);

}
}
