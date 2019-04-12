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
 * @file capabilities.hpp
 *
 * @brief Agent capability
 * */

#pragma once
#include <string>
#include <vector>

namespace psme {
namespace core {
/*! Register Agent */
namespace agent {
/*!
* @brief Capability attributes for agent
*/
class Capability {
public:
    /*!
    * @brief Create Capability object
    *
    * @param name agent name e.g. "compute"
    */
    explicit Capability(const std::string& name) :
            m_name{name} {}
    /*!
    * @brief Gets agent capability name
    *
    * @return name
    */
    const std::string& get_name() const {
        return m_name;
    }

    bool operator==(const Capability& c) const {
        return m_name == c.m_name;
    }

private:
    std::string m_name{};
};

/*! Capabilitis array */
using Capabilities = std::vector<Capability>;

}
}
}
