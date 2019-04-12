/*!
 * @brief MetricBuilder interface
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
 * @file metric_builder.hpp
 */


#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create fabric objects
 */
class MetricBuilder : public AbstractBuilder<agent_framework::model::Metric> {

public:

    using ReturnType = MetricBuilder&;

    MetricBuilder() = default;
    ~MetricBuilder() = default;

    /*!
     * @brief Updates MetricDefinition uuid in Metric
     * @param definition MetricDefinition object
     * @return Pointer to the builder
     * */
    ReturnType update_from_metric_definition(const agent_framework::model::MetricDefinition& definition);

    /*!
     * @brief Updates initial value of metric
     * @param value initial JSON value
     * @return Pointer to the builder
     * */
    ReturnType update_value(const json::Json& value);

    /*!
     * @brief Updates component related fields in Metric
     * @tparam R model resource class
     * @param resource instance of model class
     * @return Pointer to the builder
     * */
    template <typename R>
    ReturnType update_resource(const R& resource) {
        static_assert(std::is_base_of<agent_framework::model::Resource, R>::value, "Invalid resource type");
        m_obj.set_component_type(resource.get_component());
        m_obj.set_component_uuid(resource.get_uuid());
        return *this;
    }


protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;
};

}
}
}
}
