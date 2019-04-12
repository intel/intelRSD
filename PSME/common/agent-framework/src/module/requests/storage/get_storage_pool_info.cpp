/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "agent-framework/module/requests/storage/get_storage_pool_info.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetStoragePoolInfo::GetStoragePoolInfo(const std::string& pool): m_storage_pool(pool) {}

json::Json GetStoragePoolInfo::to_json() const {
    json::Json value = json::Json();
    value[StoragePool::STORAGE_POOL] = m_storage_pool;
    return value;
}

GetStoragePoolInfo GetStoragePoolInfo::from_json(const json::Json& json) {
    return GetStoragePoolInfo(json[StoragePool::STORAGE_POOL].get<std::string>());
}
