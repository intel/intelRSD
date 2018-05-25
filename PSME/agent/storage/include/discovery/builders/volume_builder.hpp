/*!
 * @brief Volume builder class interface.
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
 * @file volume_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/volume.hpp"


namespace agent_framework {
namespace model {
namespace requests {

class AddVolume;

}
}
}


namespace agent {
namespace storage {

namespace lvm {
namespace model {

class LogicalVolume;
class VolumeGroup;

}
}

namespace discovery {

class VolumeBuilder final {
public:

    /*!
     * @brief Build default volume object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default volume object.
     */
    static agent_framework::model::Volume build_default(const Uuid& parent_uuid);


    /*!
     * @brief Updates volume model with LVM data.
     * @param[in,out] volume The volume to be updated.
     * @param[in] logical_volume LVM logical volume data.
     */
    static void update_with_lvm(agent_framework::model::Volume& volume,
                                const agent::storage::lvm::model::LogicalVolume& logical_volume);


    /*!
     * @brief Updates volume model with AddVolume request data.
     * @param[in,out] volume The volume to be updated.
     * @param[in] request AddVolume request data.
     * */
    static void update_with_request(agent_framework::model::Volume& volume,
                                    const agent_framework::model::requests::AddVolume& request);


    /*!
     * @brief Add replica info to volume model.
     * @param[in,out] volume The volume to be updated.
     * @param[in] replica_volume The UUID of the replica volume.
     * @param[in] type Type of replica.
     * @param[in] role Role of replica.
     */
    static void add_replica_info(agent_framework::model::Volume& volume,
                                 const Uuid& replica_volume,
                                 const agent_framework::model::enums::ReplicaType& type,
                                 const agent_framework::model::enums::ReplicaRole& role);


    /*!
     * @brief Add source of capacity to volume object.
     * @param[in,out] volume The volume to be updated.
     * @param[in] providing_pool The UUID of the providing pool.
     */
    static void add_capacity_source(agent_framework::model::Volume& volume, const Uuid& providing_pool);


    /*!
     * @brief Update UUID pointed by replica info to new UUID.
     *
     * @param[in,out] volume The volume to be updated.
     * @param[in] old_uuid UUID to be replaced.
     * @param[in] new_uuid UUID to use as new replica.
     *
     * @warning Should be called after stabilization process.
     * */
    static void update_replica_info(agent_framework::model::Volume& volume,
                                    const Uuid& old_uuid, const Uuid& new_uuid);


    /*!
     * @brief Add volume model with system path identifier from LVM data.
     * @param[in,out] volume The volume to be updated.
     * @param[in] volume_group LVM volume group data.
     * @param[in] logical_volume LVM logical volume data.
     * */
    static void add_identifier_with_system_path(agent_framework::model::Volume& volume,
                                                const agent::storage::lvm::model::VolumeGroup& volume_group,
                                                const agent::storage::lvm::model::LogicalVolume& logical_volume);


    /*!
     * @brief Add volume model with UUID identifier.
     * @param[in,out] volume The volume to be updated.
     * */
    static void add_identifier_with_uuid(agent_framework::model::Volume& volume);


    /*!
     * @brief Updates volume model with UUID identifier.
     *
     * @param[in,out] volume The volume to be updated.
     *
     * @warning Should be called after stabilization process.
     * */
    static void update_identifier_with_uuid(agent_framework::model::Volume& volume);
};

}
}
}

