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
 * @file calculation_parameters.hpp
 */
#pragma once


#include "json-wrapper/json-wrapper.hpp"

#include <string>


namespace agent_framework {
namespace model {
namespace attribute {

/*! CalculationParameters */
class CalculationParameters {
public:
    explicit CalculationParameters();


    CalculationParameters(const std::string& source_metric,
                          const std::string& result_metric) :
        m_source_metric{source_metric}, m_result_metric{result_metric} {}


    ~CalculationParameters();


    /*! Enable copy */
    CalculationParameters(const CalculationParameters&) = default;


    CalculationParameters& operator=(const CalculationParameters&) = default;


    CalculationParameters(CalculationParameters&&) = default;


    CalculationParameters& operator=(CalculationParameters&&) = default;


    /*!
     * @brief Set calculation parameters source_metric
     * @param source_metric String with CalculationParameters source_metric
     */
    void set_source_metric(const std::string& source_metric) {
        m_source_metric = source_metric;
    }


    /*!
     * @brief Get calculation parameters source_metric
     * @return calculation parameters source_metric
     */
    const std::string get_source_metric() const {
        return m_source_metric;
    }


    /*!
     * @brief Set calculation parameters result_metric
     * @param result_metric calculation parameters result_metric
     */
    void set_result_metric(const std::string& result_metric) {
        m_result_metric = result_metric;
    }


    /*!
     * @brief Get calculation parameters result_metric
     * @return calculation parameters result_metric
     */
    const std::string get_result_metric() const {
        return m_result_metric;
    }


    /*!
     * @brief Convert CalculationParameters to JSON object
     * @return JSON representation of CalculationParameters object
     */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class CalculationParameters from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed CalculationParameters object
     */
    static CalculationParameters from_json(const json::Json& json);


private:
    std::string m_source_metric{};
    std::string m_result_metric{};

};

}
}
}
