/*!
 * @brief SPDK Storage Pool builder class interface.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file spdk_storage_pool_builder.hpp
 */

#pragma once



#include "agent-framework/discovery/builders/storage_pool_builder.hpp"
#include "spdk/model/lvol_store.hpp"

namespace agent {
namespace spdk {
namespace discovery {

class SpdkStoragePoolBuilder : public agent_framework::discovery::StoragePoolBuilder {
public:

    /*!
     * @brief Updates storage pool model with SPDK model data.
     * @param[in,out] storage_pool The storage pool to be updated.
     * @param[in] lvol_store SPDK logical volume store data.
     */
    static void update_with_spdk(agent_framework::model::StoragePool& storage_pool,
                                 const ::spdk::model::LvolStore& lvol_store);



};

}
}
}