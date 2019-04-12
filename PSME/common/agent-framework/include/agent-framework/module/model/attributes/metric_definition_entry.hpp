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
 * @file metric_definition_entry.hpp
 */


#pragma once


#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Class representing elements of Metric Definitions arrays */
class MetricDefinitionEntry {
public:
    explicit MetricDefinitionEntry();


    explicit MetricDefinitionEntry(const std::string& metric_definition) : m_metric_definition(metric_definition) {}


    ~MetricDefinitionEntry();


    /*! Enable copy */
    MetricDefinitionEntry(const MetricDefinitionEntry&) = default;


    MetricDefinitionEntry& operator=(const MetricDefinitionEntry&) = default;


    MetricDefinitionEntry(MetricDefinitionEntry&&) = default;


    MetricDefinitionEntry& operator=(MetricDefinitionEntry&&) = default;


    /*!
     * @brief setter for metric_definition attribute
     * @param metric_definition of type std::string
     */
    void set_metric_definition(const std::string& metric_definition) {
        m_metric_definition = metric_definition;
    }


    /*!
     * @brief return metric_definition of type const std::string&
     * @return the metric_definition value
     */
    const std::string& get_metric_definition() const {
        return m_metric_definition;
    }


    /*!
     * @brief Make Json from metric_definitions array entry
     * @return Json object
     */
    json::Json to_json() const;


    /*!
     * @brief make metric_definitions array entry from Json
     * @param json the Json input
     * @return new collection entry
     */
    static MetricDefinitionEntry from_json(const json::Json& json);


private:
    std::string m_metric_definition{};
};

}
}
}
