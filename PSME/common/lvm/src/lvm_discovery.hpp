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
 * @file lvm_discovery.hpp
 * @brief Lvm discovery interface
 * */

#pragma once



#include <vector>



struct lvm;
struct volume_group;
struct logical_volume;
struct physical_volume;

namespace storage {
namespace lvm {

namespace model {

class VolumeGroup;
class LogicalVolume;
class PhysicalVolume;

}

enum class DiscoveryType;

/*! @brief Class for discovering lvm in the system */
class LvmDiscovery {
public:
    using VolumeGroups = std::vector<model::VolumeGroup>;


    /*!
     * @brief Constructor
     * @param lvm Pointer to main lvm structure
     * */
    explicit LvmDiscovery(struct lvm* lvm);


    /*!
     * @brief Do discovery in the system.
     *
     * If some components are not available (ie. no logical volumes created), discovery will be cancelled.
     * To discover resources partially, please specify discovery type.
     *
     * @param[in] discovery_type Type of the discovery.
     * @return Collection of VolumeGroup
     * */
    VolumeGroups discover(DiscoveryType discovery_type);


    /*!
     * @brief Converts raw LVM data to VolumeGroup model.
     * @param handle LVM volume group handle.
     * @param name Name of VG.
     * @return Volume group.
     */
    model::VolumeGroup make_volume_group(volume_group* handle, const char* name) const;


    /*!
     * @brief Converts raw LVM data to LogicalVolume model.
     * @param group LVM volume group handle.
     * @param handle LVM logical volume handle.
     * @return Logical volume.
     */
    model::LogicalVolume make_logical_volume(volume_group* group, logical_volume* handle) const;


    /*!
     * @brief Converts raw LVM data to PhysicalVolume model.
     * @param group LVM volume group handle.
     * @param handle LVM physical volume handle.
     * @return Physical volume.
     */
    model::PhysicalVolume make_physical_volume(volume_group* group, physical_volume* handle) const;


private:
    void discovery_physical_volumes(volume_group* handle, model::VolumeGroup& group);


    void discovery_logical_volumes(volume_group* handle, model::VolumeGroup& group);


    struct lvm* m_lvm{nullptr};
};

}
}

