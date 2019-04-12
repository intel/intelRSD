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
 * @file volume_group.cpp
 * @brief Volume group implementation
 * */

#include "lvm/model/volume_group.hpp"
#include "lvm/model/physical_volume.hpp"
#include "lvm/model/logical_volume.hpp"

#include <sstream>

using namespace storage::lvm::model;

const VolumeGroup::PhysicalVolumes& VolumeGroup::get_physical_volumes() const {
    return m_physical_volumes;
}

void VolumeGroup::add_physical_volume(const PhysicalVolume& volume) {
    m_physical_volumes.emplace_back(volume);
}

const VolumeGroup::LogicalVolumes& VolumeGroup::get_logical_volumes() const {
    return m_logical_volumes;
}

void VolumeGroup::add_logical_volume(const LogicalVolume& volume) {
    m_logical_volumes.emplace_back(volume);
}

std::string VolumeGroup::to_string() const {
    std::stringstream ss{};

    ss << "Volume group name: " << get_name();
    ss << "\n\tCapacity [B]: " << get_capacity_b();
    ss << "\n\tFree space [B]: " << get_free_b();
    ss << "\n\tIs protected: " << get_protection_status();
    ss << "\n\tHealth: " << get_health();
    ss << "\n\tStatus: " << get_status();

    return ss.str();
}

