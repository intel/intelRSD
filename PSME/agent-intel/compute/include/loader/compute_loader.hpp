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
 * @file loader.hpp
 * @brief Module loader interface
 * */

#ifndef AGENT_COMPUTE_LOADER_COMPUTE_LOADER_HPP
#define AGENT_COMPUTE_LOADER_COMPUTE_LOADER_HPP

#include "agent-framework/module-ref/loader/loader.hpp"

namespace agent {
namespace compute {
namespace loader {

/*!
 * @brief Loads Compute Loader.
 * Builds minor objects: hard drive, network interfaces, modules to construct
 * Compute Agent.
 */
class ComputeLoader : public agent_framework::module::loader::Loader {
public:

    /*!@brief Default destructor.*/
    virtual ~ComputeLoader() = default;

    /*!
     * @brief Loads compute configuration file and build compute.
     * @param json instance of json::Value with loaded compute configuration.
     */
    bool load(const json::Value& json) override;
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODULE_MANAGER_HPP */

