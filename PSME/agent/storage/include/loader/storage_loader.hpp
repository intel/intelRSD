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
 * @file storage_loader.hpp
 * @brief Storage module loader interface
 * */

#ifndef AGENT_STORAGE_LOADER_STORAGE_LOADER_HPP
#define AGENT_STORAGE_LOADER_STORAGE_LOADER_HPP

#include "agent-framework/module-ref/loader/loader.hpp"
#include <memory>
#include <string>

/*! forward declarations */
struct ifaddrs;

namespace agent {
namespace storage {
namespace loader {

/*!
 * @brief Storage Loader.
 */
class StorageLoader : public agent_framework::module::loader::Loader {
public:
    /*!
     * @brief Default destructor.
     */
    ~StorageLoader() = default;

    /*!
     * @brief Load configuration
     * @param[in] json Json configuration file
     * @return true if success otherwise false
     */
    bool load(const json::Value& json) override;

    /*!
     * @brief Read module configuration file
     * @param[in] json Json configuration file
     * @return true if success otherwise false
     */
    bool read_modules(const json::Value& json);

    /*!
     * @brief Make manager module
     * @param[in] json Module json configuration
     * @return true if success otherwise false
     */
    bool make_manager_module(const json::Value& json);

    /*!
     * @brief Make submodule
     * @param[in] json Submodule json configuration
     * @param[in] uuid Manager UUID
     * @return true if success otherwise false
     */
    bool make_submodule(const json::Value& json, const std::string& uuid);
};

}
}
}

#endif /* AGENT_STORAGE_LOADER_STORAGE_LOADER_HPP */
