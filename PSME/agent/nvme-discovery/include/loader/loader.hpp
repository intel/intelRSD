/*!
 * @brief Definition of NvmeLoader class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file loader.hpp
 */

#pragma once
#include "agent-framework/module/loader/loader.hpp"

namespace agent {
namespace nvme {
namespace loader {

/*!
 * @brief NVMe Loader.
 */
class NvmeLoader : public agent_framework::module::loader::Loader {
public:
    /*!
     * @brief Default destructor.
     */
    ~NvmeLoader() = default;

    /*!
     * @brief Load configuration
     * @param[in] json JSON configuration file
     * @return true if success otherwise false
     */
    bool load(const json::Json& json) override;
};

}
}
}

