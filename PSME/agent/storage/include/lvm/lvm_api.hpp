/*!
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
 *
 * @file lvm_api.hpp
 * @brief C++ wrapper for liblvm2app
 * */

#ifndef PSME_AGENT_STORAGE_LVM_API_HPP
#define PSME_AGENT_STORAGE_LVM_API_HPP

#include "lvm_create_data.hpp"
#include "volume_group.hpp"
#include "physical_volume.hpp"
#include "logical_volume.hpp"
#include <vector>
#include <string>
#include <memory>

namespace agent {
namespace storage {
namespace lvm {

/*!
 * @brief C++ wrapper for liblvm2app
 */
class LvmAPI {
public:
    using VolumeGroupVec = std::vector<VolumeGroup>;

    /*! @brief Constructor */
    explicit LvmAPI();
    /*! @brief Destructor */
    ~LvmAPI();

    /*!
     * @brief Discover volume groups with their physical and logical volumes
     * @return Vector of volume groups to be filled.
     * */
    VolumeGroupVec discovery_volume_groups();

    /*!
     * @brief Create snapshot clone of given logical volume
     * @param[in] data Name of volume group of logical volume to be cloned
     * */
    void create_snapshot(const LvmCreateData& data);

    /*!
     * @brief Create full clone of given logical volume. It takes time, so better run it in separate thread.
     * @param[in] data Name of volume group of logical volume to be cloned
     * */
    void create_clone(const LvmCreateData& data);

    /*!
     * @brief Remove given logical volume
     * @param[in] vg_name Name of volume group of logical volume to be removed
     * @param[in] lv_name Name of logical volume to be removed
     * */
    void remove_logical_volume(const std::string& vg_name,
                               const std::string& lv_name);

private:
    class LvmImpl;
    std::shared_ptr<LvmImpl> m_impl{};
};

}
}
}
#endif	/* PSME_AGENT_STORAGE_LVM_API_HPP */

