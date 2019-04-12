/*!
 * @brief Volume builder class interface.
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
 * @file volume_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/volume.hpp"



namespace agent_framework {
namespace discovery {

class VolumeBuilder {
public:

    /*!
     * @brief Build default volume object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default volume object.
     */
    static agent_framework::model::Volume build_default(const Uuid& parent_uuid);


    /*!
     * @brief Add replica info to volume model.
     * @param[in,out] volume The volume to be updated.
     * @param[in] type Type of replica.
     * @param[in] role Role of replica.
     * @param[in] replica_volume_uuid Optional UUID of the replica volume.
     * @param[in] access Optional replica access information.
     */
    static void add_replica_info(model::Volume& volume,
                                 const model::enums::ReplicaType& type,
                                 const model::enums::ReplicaRole& role,
                                 const OptionalField<Uuid>& replica_volume_uuid,
                                 const OptionalField<model::enums::ReplicaReadOnlyAccess>& access = {});


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
     * @brief Set source of capacity of volume object.
     * @param[in,out] volume The volume to be updated.
     * @param[in] providing_pool The UUID of the providing pool.
     */
    static void set_storage_pool_capacity_source(agent_framework::model::Volume& volume, const Uuid& providing_pool);

};

}
}

