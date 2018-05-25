/*!
 * @brief Storage pool builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file storage_pool_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/storage_pool.hpp"
#include "lvm/lvm_api.hpp"



namespace agent {
namespace storage {
namespace discovery {

class StoragePoolBuilder final {
public:

    /*!
     * @brief Build default storage pool object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default storage pool object.
     */
    static agent_framework::model::StoragePool build_default(const Uuid& parent_uuid);


    /*!
     * @brief Updates storage pool model with LVM data.
     * @param storage_pool Storage pool to be updated.
     * @param volume_group LVM volume group data.
     */
    static void update_with_lvm(agent_framework::model::StoragePool& storage_pool,
                                const agent::storage::lvm::model::VolumeGroup& volume_group);


    /*!
     * @brief Adds capacity source entry to storage pool object.
     * @param storage_pool Storage pool to be updated.
     * @param allocated_bytes Provided capacity of the corresponding drive.
     * @param consumed_bytes Used capacity of the corresponding drive.
     * @param drive_uuid UUID of the corresponding drive.
     */
    static void add_capacity_source(agent_framework::model::StoragePool& storage_pool,
                                    std::uint64_t allocated_bytes,
                                    std::uint64_t consumed_bytes,
                                    const Uuid& drive_uuid);

    /*!
     * @brief Add pool model with system path identifier from LVM data.
     * @param[in,out] storage_pool The storage pool to be updated.
     * @param[in] volume_group LVM volume group data.
     * */
    static void add_identifier_with_system_path(agent_framework::model::StoragePool& storage_pool,
                                                const agent::storage::lvm::model::VolumeGroup& volume_group);


    /*!
     * @brief Add pool model with UUID identifier.
     * @param[in,out] storage_pool The storage pool to be updated.
     * */
    static void add_identifier_with_uuid(agent_framework::model::StoragePool& storage_pool);


    /*!
     * @brief Updates pool model with UUID identifier.
     *
     * @param[in,out] storage_pool The storage pool to be updated.
     *
     * @warning Should be called after stabilization process.
     * */
    static void update_identifier_with_uuid(agent_framework::model::StoragePool& storage_pool);
};

}
}
}

