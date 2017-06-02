/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file manager_and_chassis.hpp
 *
 * @brief smbios-based discovery of manager and chassis
 * */

#pragma once

#include "smbios/smbios_parser.hpp"
#include "agent-framework/module/model/manager.hpp"

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

/*!
 * @brief Fills in Manager object with data retrieved from SMBIOS
 *
 * @param[in] smbios Reference to SMBIOS parser
 * @param[in] manager reference to manager model to be filled in with data
 * @param[out] fru_info fru info data to be set on chassis
 * @return status
 * */
bool discover_manager(::smbios::parser::SmbiosParser::Ptr smbios,
                      agent_framework::model::Manager& manager,
                      agent_framework::model::attribute::FruInfo& fru_info);

/*!
 * @brief Fills in Chassis object with data retrieved from SMBIOS
 *
 * @param[in] smbios Reference to SMBIOS parser
 * @param[in] manager Reference to manager model to be filled in with data
 * @param[out] out_chassis_uuid chassis' uuid
 * @return status of the operation
 */
bool discover_all_chassis(::smbios::parser::SmbiosParser::Ptr smbios,
                          agent_framework::model::Manager& manager,
                          std::string& out_chassis_uuid);

/*!
 * @brief Sets manager guid
 *
 * @param[in] smbios Reference to SMBIOS parser
 * @param[in] manager Reference to manager model to be filled in with data
 * @return status of the operation
 */
bool discover_manager_guid(::smbios::parser::SmbiosParser::Ptr smbios, agent_framework::model::Manager& manager);

/*!
 * @brief Sets location offsets on all chassis objects associated with the manager
 *
 * @param[in] smbios Reference to SMBIOS parser
 * @param[in] manager Reference to manager model to be filled in with data
 * @return status of the operation
 */
bool discover_chassis_location(::smbios::parser::SmbiosParser::Ptr smbios, agent_framework::model::Manager& manager);

}
}
}
}
