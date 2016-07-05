/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
*/

#include "agent-framework/module/module.hpp"
#include "configuration/configuration.hpp"
#include "agent-framework/logger_ext.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "json/json.hpp"

#define ENABLE_CONFIGURATION_ENCRYPTION

using namespace agent_framework::generic;

namespace {
#ifdef ENABLE_CONFIGURATION_ENCRYPTION
static inline std::string decrypt_configuration_value(const std::string& value) {
    return configuration::Configuration::get_instance().decrypt_value(value);
}
#else
static inline std::string decrypt_configuration_value(const std::string& value) {
    return value;
}
#endif
}

void Module::read_configuration(const json::Value& module_configuration) {
    try {
        log_debug(GET_LOGGER("module"), "Reading module configuration.");
        if (module_configuration["type"].is_string()) {
            m_type = module_configuration["type"].as_string();
        }

        m_username = decrypt_configuration_value(
            module_configuration["username"].as_string());
        m_password = decrypt_configuration_value(
            module_configuration["password"].as_string());

        m_port = module_configuration["port"].as_uint();

        if (module_configuration["slot"].is_number()) {
            m_slot = module_configuration["slot"].as_uint();
        }

        for (const auto& sub_conf : module_configuration["submodules"].as_array()) {
            auto submodule = Submodule::make_submodule();
            submodule->read_configuration(sub_conf);
            add_submodule(std::move(submodule));
        }
    }
    catch (const json::Value::Exception& e) {
        log_warning(GET_LOGGER("module"), "Invalid/missing module configuration member: "
                  << e.what());
    }
    catch (...) {
        log_alert(GET_LOGGER("module"), "Unknown error in module section");
    }
}

void Module::add_submodule(SubmoduleUniquePtr submodule) {
    submodule->set_ip_address(get_ip_address());
    submodule->set_username(get_username());
    submodule->set_password(get_password());
    m_submodules.push_back(std::move(submodule));
}

HardDriveWeakPtr Module::find_hard_drive(const std::string& uuid) const {

    const auto& submodules = get_submodules();
    for (const auto& submodule : submodules) {
        const auto& hard_drive = submodule->find_hard_drive(uuid);
        if (!hard_drive.expired()) {
            return hard_drive;
        }
    }
    return {};
}

Target::TargetWeakPtr Module::find_target(const std::string& uuid) const {

    const auto& submodules = get_submodules();
    for (const auto& submodule : submodules) {
        const auto& target = submodule->get_target_manager().find_by_uuid(uuid);
        if (target) {
            return target;
        }
    }
    return {};
}

LogicalDriveWeakPtr Module::find_logical_drive(const std::string& uuid) const {
    const auto& submodules = get_submodules();
    for (const auto& submodule : submodules) {
        const auto& logical_drive = submodule->find_logical_drive(uuid);
        if (!logical_drive.expired()) {
            return logical_drive;
        }
    }

    return {};
}

std::vector<HardDriveSharedPtr> Module::get_hard_drives() const {
    std::vector<HardDriveSharedPtr> hard_drives;
    const auto& submodules = get_submodules();
    for (const auto& submodule : submodules) {
        auto submodule_drives = submodule->get_hard_drives();
        hard_drives.reserve(hard_drives.size() + submodule_drives.size());
        std::copy(submodule_drives.cbegin(), submodule_drives.cend(),
                  std::back_inserter(hard_drives));
    }
    return hard_drives;
}
