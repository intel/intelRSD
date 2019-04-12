/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file conversions.hpp
 *
 * @brief Converting functions for smbios structures
 * */

#pragma once

#include "smbios/structs/smbios_structs.hpp"

#include <string>



namespace smbios {
namespace utilities {


/*!
 * Retrieve system UUID from SMBIOS structure representing system
 *
 * @param[in] system_info_data SMBIOS structure representing a system
 *
 * @return Formatted system UUID
 * */
const std::string get_system_uuid(const smbios::parser::SYSTEM_INFO_DATA& system_info_data);


}
}
