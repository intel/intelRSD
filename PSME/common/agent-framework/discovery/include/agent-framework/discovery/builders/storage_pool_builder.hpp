/*!
 * @brief Storage pool builder class interface.
 *
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
 * @file storage_pool_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/storage_pool.hpp"



namespace agent_framework {
namespace discovery {

class StoragePoolBuilder {
public:

    /*!
     * @brief Build default storage pool object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default storage pool object.
     */
    static agent_framework::model::StoragePool build_default(const Uuid& parent_uuid);


    /*!
     * @brief Adds capacity source entry to storage pool object.
     * @param storage_pool Storage pool to be updated.
     * @param allocated_bytes Provided capacity of the corresponding drive.
     * @param consumed_bytes Used capacity of the corresponding drive.
     * @param drive_uuid UUID of the corresponding drive.
     */
    static void add_drive_capacity_source(agent_framework::model::StoragePool& storage_pool,
                                          std::int64_t allocated_bytes,
                                          std::int64_t consumed_bytes,
                                          const Uuid& drive_uuid);

};

}
}
