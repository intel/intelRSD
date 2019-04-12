/*!
 * @brief Definition of runtime Nvme context class.
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
 * @file nvme_context.hpp
 */

#pragma once

#include "tools/context.hpp"

namespace agent {
namespace nvme {
namespace tools {

class NvmeContext final : public Context {
public:
    /*!
     * @brief Default constructor
     */
    NvmeContext() = default;

    /*!
     * @brief Destructor
     */
    virtual ~NvmeContext();

    DatabasePtr create_database(const Uuid& resource, const std::string& name) const override;
};

}
}
}

