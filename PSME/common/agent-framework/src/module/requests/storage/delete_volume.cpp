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
 * @file delete_volume.cpp
 * @brief Storage command DeleteVolume implementation
 * */


#include "agent-framework/module/requests/storage/delete_volume.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeleteVolume::DeleteVolume(const Uuid& volume): m_volume{volume} {}

json::Json DeleteVolume::to_json() const {
    json::Json value = json::Json();
    value[Volume::VOLUME] = m_volume;
    return value;
}

DeleteVolume DeleteVolume::from_json(const json::Json& json) {
    return DeleteVolume{json[Volume::VOLUME].get<std::string>()};
}
