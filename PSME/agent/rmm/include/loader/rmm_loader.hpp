/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file rmm/loader/rmm_loader.hpp
 */

#pragma once

#include "agent-framework/module/loader/loader.hpp"
#include "discovery/helpers/common.hpp"

namespace agent {
namespace rmm {
namespace loader {

/*!
* @brief Loads Rmm Loader.
*
* */
class RmmLoader : public agent_framework::module::loader::Loader {
public:

    RmmLoader(agent::rmm::discovery::helpers::DiscoveryContext& dc): m_dc(dc) { }

    /*!@brief Default destructor.*/
    virtual ~RmmLoader() = default;

    /*!
     * @brief Loads compute configuration file and build compute.
     * @param[in] json instance of json::Json with loaded compute configuration.
     * @return true if success otherwise false
     * */
    bool load(const json::Json& json) override;

private:
    agent::rmm::discovery::helpers::DiscoveryContext& m_dc;
};

}
}
}
