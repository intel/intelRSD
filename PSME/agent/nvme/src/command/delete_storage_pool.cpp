/*!
 * @brief Implementation of DeleteStoragePool command.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file delete_storage_pool.cpp
 */


#include "nvme_agent_commands.hpp"

using namespace agent::nvme;
namespace {

void delete_storage_pool(DeleteStoragePool::ContextPtr,
                         const DeleteStoragePool::Request&,
                         DeleteStoragePool::Response&) {
    THROW(agent_framework::exceptions::InvalidValue, "nvme-agent", "Removing storage pool is not supported.");
}
}

REGISTER_NVME_COMMAND(DeleteStoragePool, ::delete_storage_pool);