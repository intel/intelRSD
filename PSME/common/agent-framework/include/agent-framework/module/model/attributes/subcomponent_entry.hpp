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
 * @brief Subcomponent specific data used in commands response
 * */

#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Class representing elements of collections arrays */
class SubcomponentEntry {
public:
    explicit SubcomponentEntry();

    SubcomponentEntry(const std::string& subcomponent) :
                      m_subcomponent(subcomponent) {}

    ~SubcomponentEntry();

    /*! Enable copy */
    SubcomponentEntry(const SubcomponentEntry&) = default;
    SubcomponentEntry& operator=(const SubcomponentEntry&) = default;
    SubcomponentEntry(SubcomponentEntry&&) = default;
    SubcomponentEntry& operator=(SubcomponentEntry&&) = default;

    void set_subcomponent(const std::string& subcomponent) {
        m_subcomponent = subcomponent;
    }

    /*!
     * @brief return an object subcomponent of class std::string
     *
     * @return the subcomponent value
     */
    const std::string& get_subcomponent() const {
        return m_subcomponent;
    }

    /*!
     * @brief Make Json from subcomponents array entry
     * @return Json object
     */
    json::Json to_json() const;

    /*!
     * @brief make subcomponents array entry from Json
     * @param[in] json the Json input
     * @return new collection entry
     */
    static SubcomponentEntry from_json(const json::Json& json);

private:

    std::string m_subcomponent{};

};

}
}
}
