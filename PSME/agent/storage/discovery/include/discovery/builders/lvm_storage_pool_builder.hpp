/*!
 * @brief LVM Storage pool builder class interface.
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
 * @file lvm_storage_pool_builder.hpp
 */

#pragma once



#include "agent-framework/discovery/builders/storage_pool_builder.hpp"



namespace storage {
namespace lvm {
namespace model {

class VolumeGroup;

}
}
}

namespace agent {
namespace storage {
namespace discovery {

class LvmStoragePoolBuilder : public agent_framework::discovery::StoragePoolBuilder {
public:

    /*!
     * @brief Updates storage pool model with LVM data.
     * @param storage_pool Storage pool to be updated.
     * @param volume_group LVM volume group data.
     */
    static void update_with_lvm(agent_framework::model::StoragePool& storage_pool,
                                const ::storage::lvm::model::VolumeGroup& volume_group);

};

}
}
}

