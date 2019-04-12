/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file utils.hpp
 * @brief Utility functions declarations
 */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"


namespace agent {
namespace storage {
namespace utils {



/*!
 * @brief Set state of the volume to starting and prevent from using.
 * @param uuid Volume's UUID.
 */
void lock_volume_for_initialization(const Uuid& uuid);


/*!
 * @brief Release the volume from locked state.
 * @param uuid Volume's UUID.
 * @param success Set state to Ok if success, warning otherwise.
 */
void unlock_volume_for_initialization(const Uuid& uuid, bool success = true);


/*!
 * @brief Helper function to update all relations for storage pool
 * @param old_uuid Old UUID of the storage pool (before update)
 * @param new_uuid New UUID of the storage pool (after update)
 */
void update_storage_pool_relations(const Uuid& old_uuid, const Uuid& new_uuid);


}
}
}

