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

#ifndef AGENT_SIMULATOR_COMPUTE_LOADER_COMPUTE_LOADER_HPP
#define AGENT_SIMULATOR_COMPUTE_LOADER_COMPUTE_LOADER_HPP

#include "agent-framework/module-ref/loader/loader.hpp"
#include "asset_configuration/asset_configuration.hpp"

namespace agent {
namespace compute {
namespace loader {

    /*!
     * @brief Class loading asset configuration into a model.
     * */
class ComputeLoader {
public:
    ~ComputeLoader();

    /**
     * @brief Loads compute module configuration.
     *
     * @param[in] node root node of DOM tree representing the asset.
     *
     * @return true if configuration was loaded correctly, false otherwise.
     */
    bool load(const xmlpp::Node* node);

};

} /* loader */
} /* compute */
} /* agent */

#endif /* AGENT_SIMULATOR_COMPUTE_LOADER_COMPUTE_LOADER_HPP */

