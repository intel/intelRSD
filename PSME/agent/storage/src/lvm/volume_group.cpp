/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file volume_group.cpp
 *
 * @brief Volume group implementation
 * */

#include "lvm/volume_group.hpp"
#include "lvm/physical_volume.hpp"
#include "lvm/logical_volume.hpp"

using namespace agent::storage::lvm;

const VolumeGroup::PhysicalVolumeVec& VolumeGroup::get_physical_volumes() const {
    return m_physical_volumes;
}

void VolumeGroup::add_physical_volume(const PhysicalVolume& volume) {
    m_physical_volumes.emplace_back(volume);
}

const VolumeGroup::LogicalVolumeVec& VolumeGroup::get_logical_volumes() const {
    return m_logical_volumes;
}

void VolumeGroup::add_logical_volume(const LogicalVolume& volume) {
    m_logical_volumes.emplace_back(volume);
}

