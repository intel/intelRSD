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
 *
 * @file network_loader.hpp
 * @brief Network module loader interface
 * */

#pragma once
#include "agent-framework/module/loader/loader.hpp"

namespace agent {
namespace network {
namespace loader {

/*!
 * @brief Network Loader.
 */
class NetworkLoader : public agent_framework::module::loader::Loader {
public:
    /*!
     * @brief Default destructor.
     */
    ~NetworkLoader() = default;

    /*!
     * @brief Load configuration
     * @param[in] json JSON configuration file
     * @return true if success otherwise false
     */
    bool load(const json::Json& json) override;

    /*!
     * @brief Read FabricModule configuration file
     * @param[in] json JSON configuration file
     */
    void read_fabric_modules(const json::Json& json);
};

}
}
}

