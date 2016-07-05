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
 * @brief Subcomponent specific data used in commands response
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_SUBCOMPONENT_ENTRY_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_SUBCOMPONENT_ENTRY_HPP

#include "agent-framework/module-ref/enum/common.hpp"
#include <string>

namespace Json {
class Value;
}

namespace agent_framework {
namespace model {
namespace attribute {

/*! Class representing elements of collections arrays
 * */
class SubcomponentEntry {

public:

    explicit SubcomponentEntry();

    SubcomponentEntry(const std::string& subcomponent):
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
    Json::Value to_json() const;

    /*!
     * @brief make subcomponents array entry from Json
     * @param[in] json the Json input
     * @return new collection entry
     */
    static SubcomponentEntry from_json(const Json::Value& json);

private:

    std::string m_subcomponent{};

};

}
}
}
#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_SUBCOMPONENT_ENTRY_HPP */
