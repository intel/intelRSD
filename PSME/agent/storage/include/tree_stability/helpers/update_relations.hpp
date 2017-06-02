/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
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
 * @file update_relations.hpp
 * */

#pragma once



#include <string>



namespace agent {
namespace storage {
namespace helpers {

/*!
 * Update all relations in the model which involve switch logical_drives
 *
 * @param[in] logical_drive_temporary_uuid Logical drive UUID before tree stabilization action
 * @param[in] logical_drive_persistent_uuid Logical drive UUID after tree stabilization action
 * */
void update_logical_drive_in_relations(const std::string& logical_drive_temporary_uuid,
                                       const std::string& logical_drive_persistent_uuid);


/*!
 * Update all relations in the model which involve iSCSI targets
 *
 * @param[in] target_temporary_uuid target UUID before tree stabilization action
 * @param[in] target_persistent_uuid target UUID after tree stabilization action
 * */
void update_target_in_relations(const std::string& target_temporary_uuid, const std::string target_persistent_uuid);

}
}
}
