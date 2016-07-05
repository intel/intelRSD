/*!
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
 *
 * @file module_manager.hpp
 * @brief Module Manager
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODULE_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_MODULE_MANAGER_HPP

#include "uuid.h"

#include <algorithm>
#include <exception>
#include <vector>
#include <cstdio>

#include "module.hpp"
#include "chassis.hpp"

namespace json { class Value; }

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! Module Manager */
class ModuleManager {
public:
    /*! Vector of Modules. */
    using module_vec_t = std::vector<agent_framework::generic::Module::ModuleUniquePtr>;

    /*! Default constructor. */
    ModuleManager() {}

    /*! Destructor */
    ~ModuleManager();

    /*!
     * Allocates Modules and pushes them to given vector.
     *
     * @throw invalida_argument Exception.
     * */
    static void create();

    /*!
     * Adds module to vector
     *
     * @param[in] module Pointer to module
     * */
    static void add(const Module* module);

    /*!
     * Deletes all allocated modules in given vector.
     * */
    static void cleanup();

    /*!
     *  Gets vector of modules.
     *
     *  @return Vector of modules.
     * */
    static module_vec_t& get_modules();

    /*!
     * Gets module with given UUID.
     *
     * @param[in] uuid UUID of module.
     *
     * @return Module with given UUID.
     * */
    static Module* get_module(const std::string& uuid);

    /*!
     * Gets submodule with given UUID.
     *
     * @param[in] uuid UUID of submodule.
     *
     * @return Submodule with given UUID.
     * */
    static Submodule* get_submodule(const std::string& uuid);

    /*!
     * Gets hard drive with given UUID.
     *
     * @param[in] uuid UUID of hard drive.
     *
     * @return Hard drive with given UUID.
     * */
    static HardDriveWeakPtr find_hard_drive(const std::string& uuid);

    /*!
     * Gets logical drive with given UUID.
     *
     * @param[in] uuid UUID of logical drive.
     *
     * @return Logical drive with given UUID.
     * */
    static LogicalDriveWeakPtr find_logical_drive(const std::string& uuid);

    /*!
     * Gets target with given UUID.
     *
     * @param[in] uuid UUID of target.
     *
     * @return Target with given UUID.
     * */
    static Target::TargetWeakPtr find_target(const std::string& uuid);

    /*!
     * @brief Returns vector of hard drives.
     * Note, that this method returns all hard drives from
     * all modules.
     *
     * @return Vector of hard drives.
     * */
    static std::vector<HardDriveSharedPtr> get_hard_drives();

    static std::vector<StorageController*> get_storage_controllers();

    static const Processor& get_processor(const std::string& uuid);

    static const Memory& get_memory(const std::string& uuid);

    static const StorageController*
                                get_storage_controller(const std::string& uuid);

    static const NetworkInterface*
                                get_network_interface(const std::string& uuid);

    /*!
     *  Gets chassis.
     *
     *  @return Chassis.
     * */
    static Chassis* get_chassis();

private:

    static void m_create_chassis(const json::Value& configuration);
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODULE_MANAGER_HPP */

