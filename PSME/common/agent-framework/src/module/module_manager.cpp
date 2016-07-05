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

#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "agent-framework/logger_ext.hpp"
#include "configuration/configuration.hpp"

using configuration::Configuration;

using namespace std;
using namespace agent_framework::generic;

using ModuleUniquePtr = Module::ModuleUniquePtr;

static ModuleManager::module_vec_t* g_modules = nullptr;
static Chassis* g_chassis = nullptr;

ModuleManager::~ModuleManager() {
    delete g_modules;
    g_modules = nullptr;

    delete g_chassis;
    g_chassis = nullptr;
}

void ModuleManager::create() {

    const json::Value& configuration = Configuration::get_instance().to_json();

    if (!configuration["modules"].is_array()) {
        log_error(GET_LOGGER("module"), "Modules from configuration is not an array");
        return;
    }

    module_vec_t& modules = get_modules();

    for (const auto& module_json : configuration["modules"].as_array()) {
        if (module_json.is_object()) {
            if (module_json["ipv4"].is_string()) {
                auto module = Module::make_module(module_json["ipv4"].as_string());
                module->read_configuration(module_json);
                modules.push_back(std::move(module));

                log_debug(GET_LOGGER("module"), "Created module with ipv4: "
                        << module_json["ipv4"].as_string());
            }
        }
    }

    m_create_chassis(configuration);
}

void ModuleManager::m_create_chassis(const json::Value& configuration) {

    if (nullptr == g_chassis && configuration["chassis"].is_object()) {
        g_chassis = new Chassis();
        g_chassis->read_configuration(configuration["chassis"]);
    }
}

void ModuleManager::cleanup() {
    if (nullptr == g_modules) {
        return;
    }
    g_modules->clear();
}

ModuleManager::module_vec_t& ModuleManager::get_modules() {
    if (nullptr == g_modules) {
        g_modules = new module_vec_t();
    }
    return *g_modules;
}

Module* ModuleManager::get_module(const std::string& uuid) {
    auto& modules = ModuleManager::get_modules();
    auto it = std::find_if(modules.begin(), modules.end(),
                                             [&uuid](const ModuleUniquePtr& m) {
        return (uuid.compare(m->get_name()) == 0); });

    if (it == modules.end()) {
        return nullptr;
    }

    return it->get();
}

Submodule* ModuleManager::get_submodule(const std::string& uuid) {
    auto& modules = ModuleManager::get_modules();
    for (auto& module : modules) {
        auto& submodules = module->get_submodules();
        auto it = std::find_if(submodules.begin(), submodules.end(),
                                        [&uuid](const SubmoduleUniquePtr& sm) {
            return (uuid.compare(sm->get_name()) == 0); });
        if (it != submodules.end()) {
            return it->get();
        }
    }
    return nullptr;
}

HardDriveWeakPtr ModuleManager::find_hard_drive(const std::string& uuid) {

    const auto& modules = ModuleManager::get_modules();
    for (const auto& module : modules) {
        const auto& hard_drive = module->find_hard_drive(uuid);
        if (!hard_drive.expired()) {
            return hard_drive;
        }
    }
    return {};
}

LogicalDriveWeakPtr ModuleManager::find_logical_drive(const std::string& uuid) {
    const auto& modules = ModuleManager::get_modules();
    for (const auto& module : modules) {
        const auto& logical_drive = module->find_logical_drive(uuid);
        if (!logical_drive.expired()) {
            return logical_drive;
        }
    }
    return {};
}

Target::TargetWeakPtr ModuleManager::find_target(const std::string& uuid) {

    const auto& modules = ModuleManager::get_modules();
    for (const auto& module : modules) {
        const auto& target = module->find_target(uuid);
        if (!target.expired()) {
            return target;
        }
    }
    return {};
}

std::vector<HardDriveSharedPtr> ModuleManager::get_hard_drives() {
    std::vector<HardDriveSharedPtr> hard_drives;
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        auto module_drives = module->get_hard_drives();
        hard_drives.reserve(hard_drives.size() + module_drives.size());
        std::copy(module_drives.cbegin(), module_drives.cend(),
                  std::back_inserter(hard_drives));
    }
    return hard_drives;
}

Chassis* ModuleManager::get_chassis() {
    return g_chassis;
}

const Processor& ModuleManager::get_processor(const std::string& uuid) {
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        const auto& submodules = module->get_submodules();
        for (const auto& submodule : submodules) {
            if (submodule->get_processor_manager().find_elem(uuid)) {
                return submodule->get_processor_manager().get_entry(uuid);
            }
        }

    }

    THROW(::agent_framework::exceptions::InvalidUuid, "rpc",
      "Processor not found, invalid uuid " + uuid);
}

const Memory& ModuleManager::get_memory(const std::string& uuid) {
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        const auto& submodules = module->get_submodules();
        for (const auto& submodule : submodules) {
            if (submodule->get_memory_manager().find_elem(uuid)) {
                return submodule->get_memory_manager().get_entry(uuid);
            }
        }

    }

    THROW(::agent_framework::exceptions::InvalidUuid, "rpc",
      "Memory not found, invalid uuid " + uuid);
}

std::vector<StorageController*> ModuleManager::get_storage_controllers() {
    std::vector<StorageController*> storage_controllers_vec;
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        const auto& submodules = module->get_submodules();
        for (const auto& submodule : submodules) {
            auto& storage_controllers =
                                        submodule->get_storage_controllers();
            for (const auto& sc : storage_controllers) {
                storage_controllers_vec.emplace_back(sc.get());
            }
        }
    }
    return storage_controllers_vec;
}

const StorageController*
ModuleManager::get_storage_controller(const std::string& uuid) {
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        const auto& submodules = module->get_submodules();
        for (const auto& submodule : submodules) {
            const auto& storage_controllers =
                                        submodule->get_storage_controllers();
            for (const auto& sc : storage_controllers) {
                if (sc->get_uuid() == uuid) {
                    return sc.get();
                }
            }
        }
    }

    THROW(::agent_framework::exceptions::InvalidUuid, "rpc",
      "Storage Controller not found, invalid uuid " + uuid);
}
const NetworkInterface*
ModuleManager::get_network_interface(const std::string& uuid) {
    const auto& modules = get_modules();
    for (const auto& module : modules) {
        const auto& submodules = module->get_submodules();
        for (const auto& submodule : submodules) {
            const auto& network_interfaces =
                                        submodule->get_network_interfaces();
            for (const auto& nic : network_interfaces) {
                if (nic->get_uuid() == uuid) {
                    return nic.get();
                }
            }
        }
    }

    THROW(::agent_framework::exceptions::InvalidUuid, "rpc",
      "Network interface not found, invalid uuid " + uuid);
}
