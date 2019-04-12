/*!
 * @brief RegistryConfigurator class implementation
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file registry_configurator.cpp
 * */


#include "psme/rest/registries/config/registry_configurator.hpp"
#include "psme/rest/registries/managers/message_registry_file_manager.hpp"
#include "psme/rest/registries/managers/message_registry_manager.hpp"



namespace psme {
namespace rest {
namespace registries {

namespace {

MessageRegistryFile::Languages get_languages(const json::Json& message_registries) {
    MessageRegistryFile::Languages languages{};

    for (const auto& registry : message_registries) {
        languages.push_back(registry["Language"]);
    }

    return languages;
}


MessageRegistryFile::Locations get_external_locations(const json::Json& message_registries) {
    MessageRegistryFile::Locations external_locations{};

    for (const auto& registry : message_registries) {
        Location location = Location::from_json(registry);
        external_locations.emplace_back(location);
    }

    return external_locations;
}

}


RegistryConfigurator::~RegistryConfigurator() { }


void RegistryConfigurator::load(const std::string& configuration) {
    std::lock_guard<std::mutex> lock{m_mutex};

    json::Json json = json::Json::parse(configuration);

    for (const auto& message_registry_json : json["MessageRegistryFiles"]) {
        MessageRegistryFile message_registry_file{};

        message_registry_file.set_name(message_registry_json["Name"]);
        message_registry_file.set_description(message_registry_json["Description"]);
        message_registry_file.set_registry(message_registry_json["Registry"]);
        message_registry_file.set_languages(get_languages(message_registry_json["MessageRegistries"]));
        message_registry_file.set_locations(get_external_locations(message_registry_json["MessageRegistries"]));

        MessageRegistryFileManager::get_instance()->add_registry_file(message_registry_file);
    }
}

}
}
}
