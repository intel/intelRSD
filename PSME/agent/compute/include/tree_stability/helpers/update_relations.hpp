/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
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
 * @file update_relations.hpp
 * */

#pragma once

#include <string>



namespace agent {
namespace compute {
namespace helpers {

/*!
 * Update all relations in the model which involve chassis
 *
 * @param[in] chassis_temporary_uuid Chassis UUID before tree stabilization action
 * @param[in] chassis_persistent_uuid Chassis UUID after tree stabilization action
 * */
void update_chassis_in_relations(const std::string& chassis_temporary_uuid, const std::string chassis_persistent_uuid);

}
}
}
