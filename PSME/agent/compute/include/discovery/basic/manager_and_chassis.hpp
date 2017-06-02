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
 * @brief ipmi-based (legacy mode) discovery of manager and chassis
 * */

#pragma once

#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/module/model/attributes/fru_info.hpp"
#include "agent-framework/module/enum/common.hpp"
#include <mutex>

namespace ipmi {
class ManagementController;
}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

/*!
 * @brief Fills in Manager object with data retrieved via IPMI
 *
 * Method uses get_device_id IPMI command
 *
 * @param[in] mc Reference to management controller
 * @param[in] manager reference to manager model to be filled in with data
 * @param[out] fru_info fru info data to be set on chassis
 * @return tuple(status, health to be set on manager)
 * */
std::tuple<bool, agent_framework::model::enums::Health> discover_manager(ipmi::ManagementController& mc,
                                                                         agent_framework::model::Manager& manager,
                                                                         agent_framework::model::attribute::FruInfo& fru_info);

/*!
 * @brief Fills in Manager object with data retrieved via IPMI
 *
 * Method uses get_device_id IPMI command
 *
 * @param[in] mc Reference to management controller
 * @param[in] manager reference to manager model to be filled in with data
 * @param[in] fru_info fru info data to be set on chassis
 * @param[out] out_chassis_uuid chassis' uuid
 * @return tuple(status, health to be set on manager)
 * */
std::tuple<bool, agent_framework::model::enums::Health> discover_all_chassis(ipmi::ManagementController& mc,
                                                                             agent_framework::model::Manager& manager,
                                                                             const agent_framework::model::attribute::FruInfo& fru_info,
                                                                             std::string& out_chassis_uuid);

/*!
 * @brief Sets manager guid
 *
 * Method uses get_device_guid IPMI command
 *
 * @param[in] mc Reference to management controller
 * @param[in] manager reference to manager model to be filled in with data
 * @return tuple(status, health to be set on manager)
 * */
std::tuple<bool, agent_framework::model::enums::Health> discover_manager_guid(ipmi::ManagementController& mc,
                                                                              agent_framework::model::Manager& manager);

/*!
 * @brief Sets location offsets on all chassis objects associated with the manager
 *
 * Method uses get_slot_id IPMI command
 *
 * @param[in] mc Reference to management controller
 * @param[in] manager reference to manager model to be filled in with data
 * @return tuple(status, health to be set on manager)
 * */
std::tuple<bool, agent_framework::model::enums::Health> discover_chassis_location(ipmi::ManagementController& mc,
                                                                                  agent_framework::model::Manager& manager);

}
}
}
}
