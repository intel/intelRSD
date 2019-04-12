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
 * @file metric_definition_handler.hpp
 */

#pragma once

#include "agent-framework/module/model/metric_definition.hpp"
#include "agent-framework/module/requests/common/get_metric_definitions_collection.hpp"
#include "agent-framework/module/requests/common/get_metric_definition_info.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using agent_framework::model::requests::GetMetricDefinitionInfo;
using agent_framework::model::MetricDefinition;
using MetricDefinitionHandlerBase = GenericHandler<GetMetricDefinitionInfo, MetricDefinition,
IdPolicy<agent_framework::model::enums::Component::MetricDefinition, NumberingZone::SHARED>>;

/*! @brief MetricDefinitionHandler template specialization */
class MetricDefinitionHandler : public MetricDefinitionHandlerBase {
public:
    MetricDefinitionHandler() : MetricDefinitionHandlerBase() {}


    virtual ~MetricDefinitionHandler();



    Array<SubcomponentEntry> fetch_sibling_uuid_list(Context& ctx,
                                                     const std::string& parent_uuid,
                                                     const std::string& collection_name) override {
        using Definitions = agent_framework::model::attribute::Array<agent_framework::model::attribute::MetricDefinitionEntry>;
        using CollectionName = agent_framework::model::enums::CollectionName;
        CollectionName metric_definitions = CollectionName::MetricDefinitions;

        if (ctx.get_parent_component() != Component::None || !parent_uuid.empty()  ||
            collection_name != metric_definitions.to_string()) {
            log_error("rest", std::string("Logic error. Tried to poll list of ") + collection_name + " under parent " +
                                     + ctx.get_parent_component().to_string() + " with uuid " + parent_uuid + ".");
            return Array<SubcomponentEntry>();
        }

        try {
            log_debug("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Fetching list of all metric definitions from agent "
                << ctx.agent->get_gami_id());

            agent_framework::model::requests::GetMetricDefinitionsCollection collection{};
            auto metric_definition_entries = ctx.agent->execute<Definitions>(collection);

            // Convert array of MetricDefinitionEntry to Array of SubcomponentEntry
            Array<SubcomponentEntry> res;
            for (const auto& entry : metric_definition_entries) {
                res.add_entry(entry.get_metric_definition());
            }
            return res;
        }
        catch (const json_rpc::JsonRpcException& e) {
            log_error("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Agent exception while fetching list of all components of "
                    "type [" << component_s() << "] for parent "
                << parent_uuid << e.what());
            return Array<SubcomponentEntry>();
        }
    }
};

MetricDefinitionHandler::~MetricDefinitionHandler() {}

}
}
}
}
