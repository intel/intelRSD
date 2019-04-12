/*!
 * @brief MetricDefinitionBuilder interface
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
 * @file metric_definition_builder.hpp
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
class MetricDefinitionBuilder : public AbstractBuilder<agent_framework::model::MetricDefinition> {

public:

    using ReturnType = MetricDefinitionBuilder&;

    MetricDefinitionBuilder();
    virtual ~MetricDefinitionBuilder();


    /*!
     * @brief Builder prepare object with properties for fabric port Health metrics
     */
    ReturnType prepare_port_health_definition();

protected:
    /*!
     * @brief Overridden, because it's abstract in parent class
     * */
    virtual void build_default() override { }
};

}
}
}
}
