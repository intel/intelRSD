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
 * @file get_metrics_collection.hpp
 */


#pragma once



#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

class GetMetrics {
public:
    explicit GetMetrics();


    GetMetrics(const std::string& metric_uuid);


    /*!
     * Get command name
     * @return Command name
     * */
    static std::string get_command() {
        return model::literals::Command::GET_METRICS;
    }


    /*!
     * Convert request object to Json:Value
     * @return Json object converted from request object
     * */
    json::Json to_json() const;


    /*!
     * Create request object from Json
     * @param json Json object to construct request from
     * */
    static GetMetrics from_json(const json::Json& json);


    /*!
     * @brief Get filter metric uuid
     * @return filter metric uuid
     * */
    const OptionalField<std::string>& get_metric() const {
        return m_metric;
    }


    /*!
     * @brief Set filter metric uuid
     * @param[in] metric the filter metric uuid
     * */
    void set_metric(const OptionalField<std::string>& metric) {
        m_metric = metric;
    }


    /*!
     * @brief Get filter component uuid
     * @return filter component uuid
     * */
    const OptionalField<std::string>& get_component() const {
        return m_component;
    }


    /*!
     * @brief Set filter component uuid
     * @param[in] component the filter component uuid
     * */
    void set_component(const OptionalField<std::string>& component) {
        m_component = component;
    }


    /*!
     * @brief Get filter metric definition
     * @return filter metric definition
     * */
    const OptionalField<std::string>& get_metric_definition() const {
        return m_metric_definition;
    }


    /*!
     * @brief Set filter metric definition
     * @param[in] metric_definition the filter metric definition
     * */
    void set_metric_definition(const OptionalField<std::string>& metric_definition) {
        m_metric_definition = metric_definition;
    }


    /*!
     * @brief Get filter metric name
     * @return filter metric name
     * */
    const OptionalField<std::string>& get_name() const {
        return m_name;
    }


    /*!
     * @brief Set filter metric name
     * @param[in] name the filter metric name
     * */
    void set_name(const OptionalField<std::string>& name) {
        m_name = name;
    }


    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Metric::METRIC, VALID_OPTIONAL(VALID_NULLABLE(VALID_UUID)),
            literals::Metric::COMPONENT, VALID_OPTIONAL(VALID_NULLABLE(VALID_UUID)),
            literals::MetricDefinition::METRIC_DEFINITION, VALID_OPTIONAL(VALID_NULLABLE(VALID_UUID)),
            literals::Metric::NAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
            nullptr
        };
        return procedure;
    }


private:
    OptionalField<std::string> m_metric{};
    OptionalField<std::string> m_component{};
    OptionalField<std::string> m_metric_definition{};
    OptionalField<std::string> m_name{};
};

}
}
}
