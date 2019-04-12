/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file namespace.cpp
 */

#include "spdk/model/namespace.hpp"



using namespace spdk;

constexpr const char model::Namespace::NSID[];
constexpr const char model::Namespace::BDEV_NAME[];
constexpr const char model::Namespace::NGUID[];
constexpr const char model::Namespace::EUI64[];
constexpr const char model::Namespace::UUID[];


void model::from_json(const json::Json& json, model::Namespace& nvmf_namespace) {
    nvmf_namespace.set_bdev_name(json.at(model::Namespace::BDEV_NAME));

    nvmf_namespace.set_eui_64(json.value(model::Namespace::EUI64, json::Json{}).get<JsonOptional<std::string>>());
    nvmf_namespace.set_nguid(json.value(model::Namespace::NGUID, json::Json{}).get<JsonOptional<std::string>>());
    nvmf_namespace.set_uuid(json.value(model::Namespace::UUID, json::Json{}).get<JsonOptional<std::string>>());
    nvmf_namespace.set_namespace_id(json.value(model::Namespace::NSID, json::Json{}).get<JsonOptional<std::uint32_t>>());
}


void model::to_json(json::Json& json, const model::Namespace& nvmf_namespace) {

    json = json::Json{
        {model::Namespace::BDEV_NAME, nvmf_namespace.get_bdev_name()}
    };

    if (nvmf_namespace.get_nguid()) {
        json[model::Namespace::NGUID] = nvmf_namespace.get_nguid();
    }
    if (nvmf_namespace.get_uuid()) {
        json[model::Namespace::UUID] = nvmf_namespace.get_uuid();
    }
    if (nvmf_namespace.get_namespace_id()) {
        json[model::Namespace::NSID] = nvmf_namespace.get_namespace_id();
    }
    if (nvmf_namespace.get_eui_64()) {
        json[model::Namespace::EUI64] = nvmf_namespace.get_eui_64();
    }
}


std::string model::Namespace::to_string() const {
    std::stringstream ss{};

    ss << "    " << "BDEV name " << get_bdev_name() << std::endl;
    ss << "    " << "UUID: " << get_uuid().value_or_default() << std::endl;
    ss << "    " << "Namespace ID: " << get_namespace_id().value_or_default() << std::endl;
    ss << "    " << "Eui64: " << get_eui_64().value_or_default() << std::endl;
    ss << "    " << "NGUID: " << get_nguid().value_or_default() << std::endl;

    return ss.str();
}
