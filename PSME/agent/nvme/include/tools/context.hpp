/*!
 * @brief Definition of runtime context class.
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
 * @file context.hpp
 */

#pragma once

#include "tools/database.hpp"
#include "tools/dir.hpp"

#include <memory>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * @brief Default runtime context for the tools. We allow for inheritance as the unit tests will have its own context.
 */
class Context {
public:
    using DatabasePtr = std::shared_ptr<Database>;
    using DirPtr = std::shared_ptr<Dir>;

    /*!
     * @brief Destructor
     */
    virtual ~Context() {}

    /*!
     * @brief Create database object for given uuid and name
     *
     * @param resource Resource UUID
     * @param name name for the database
     *
     * @return Pointer to the database object
     */
    virtual DatabasePtr create_database(const Uuid& resource, const std::string& name) const = 0;

    /*!
     * @brief Get object of Dir class.
     *
     * @param Pointer to Dir object
     */
    virtual DirPtr create_dir() const = 0;
};

}
}
}

