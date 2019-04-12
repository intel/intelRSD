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
 * @file lvm_api.hpp
 * @brief C++ wrapper for liblvm2app
 * */

#pragma once

#include "model/creation_data.hpp"
#include "model/volume_group.hpp"
#include "model/physical_volume.hpp"
#include "model/logical_volume.hpp"
#include "model/discovery_type.hpp"

#include <vector>
#include <string>
#include <memory>
#include <mutex>

namespace storage {
namespace lvm {

/*!
 * @brief C++ wrapper for liblvm2app
 */
class LvmApi {
public:
    using VolumeGroups = std::vector<model::VolumeGroup>;


    /*! @brief Constructor */
    explicit LvmApi();


    /*! @brief Destructor */
    ~LvmApi();


    /*!
     * @brief Discover volume groups with their physical and logical volumes
     * @param[in] discovery_type Type of the discovery.
     * @return Vector of volume groups to be filled.
     * */
    VolumeGroups discover_volume_groups(DiscoveryType discovery_type);


    /*!
     * @brief Create snapshot clone of given logical volume
     * @param[in] data Name of volume group of logical volume to be cloned
     * @return Returns newly created snapshot volume.
     * */
    model::LogicalVolume create_snapshot(const model::CreationData& data);


    /*!
     * @brief Create empty volume.
     *
     * This method could be used to perform volume clone creation.
     * It takes time, so better run it in separate thread.
     *
     * @param[in] data Name of volume group of logical volume to be cloned.
     * @return Returns newly created volume.
     * */
    model::LogicalVolume create_volume(const model::CreationData& data);


    /*!
     * @brief Remove given logical volume
     * @param[in] vg_name Name of volume group of logical volume to be removed
     * @param[in] lv_name Name of logical volume to be removed
     * */
    void remove_logical_volume(const std::string& vg_name,
                               const std::string& lv_name);


    /*!
     * @brief Add tag to logical volume
     * @param[in] vg_name Name of volume group of logical volume to be tagged
     * @param[in] lv_name Name of logical volume to be tagged
     * @param[in] tag Tag to be added
     * */
    void add_logical_volume_tag(const std::string& vg_name,
                                const std::string& lv_name,
                                const std::string& tag);


    /*!
     * @brief Remove tag from logical volume
     * @param[in] vg_name Name of volume group of logical volume to be untagged
     * @param[in] lv_name Name of logical volume to be untagged
     * @param[in] tag Tag to be removed
     * */
    void remove_logical_volume_tag(const std::string& vg_name,
                                   const std::string& lv_name,
                                   const std::string& tag);


    /*!
     * @brief Resize the existing volume.
     * @param vg_name Name of volume group of logical volume to be resized.
     * @param lv_name Name of logical volume to be resized.
     * @param size New size of the volume.
     * @return Updated LogicalVolume model.
     */
    model::LogicalVolume resize_volume(const std::string& vg_name,
                                       const std::string& lv_name,
                                       std::uint64_t size);

private:
    class LvmImpl;
    std::shared_ptr<LvmImpl> m_impl{};

    static std::mutex g_lvm_mutex;
};

}
}
