/*!
 * @brief Implementation of DeleteStoragePool request.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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

#include "agent-framework/module/requests/storage/delete_storage_pool.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeleteStoragePool::DeleteStoragePool(const Uuid& storage_pool): m_storage_pool{storage_pool} {}

json::Json DeleteStoragePool::to_json() const {
    json::Json value = json::Json();
    value[StoragePool::STORAGE_POOL] = m_storage_pool;
    return value;
}

DeleteStoragePool DeleteStoragePool::from_json(const json::Json& json) {
    return DeleteStoragePool{json[StoragePool::STORAGE_POOL].get<std::string>()};
}
