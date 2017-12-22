/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once
#include "agent-framework/module/loader/loader.hpp"
#include "agent-framework/module/model/storage_service.hpp"
#include "agent-framework/module/model/manager.hpp"
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
     * @brief Read storage configuration file
     * @param[in] json Json configuration file
     */
    void read_managers(const json::Value& json);

    /*!
     * @brief Make manager model
     * @param[in] json Manager json configuration
     */
    agent_framework::model::Manager make_manager(const json::Value& json);

    /*!
     * @brief Make storage service model
     * @param[in] json Storage service json configuration
     * @param[in] uuid Manager UUID
     */
    agent_framework::model::StorageService
    make_storage_service(const json::Value& json, const std::string& uuid);

private:
    struct IfAddrsDeleter {
        void operator()(struct ifaddrs *) const;
    };
    using IfAddrsPointer = std::unique_ptr<struct ifaddrs, IfAddrsDeleter>;

    const std::string get_iface_ipaddress(const std::string& iface) const;
};

}
}
}

