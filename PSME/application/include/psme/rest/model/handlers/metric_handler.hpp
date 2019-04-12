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
 * @file metric_handler.hpp
 */

#pragma once

#include "agent-framework/module/model/metric.hpp"
#include "agent-framework/module/requests/common/get_metrics.hpp"


namespace psme {
namespace rest {
namespace model {
namespace handler {

using agent_framework::model::requests::GetMetrics;
using agent_framework::model::Metric;

using MetricHandlerBase = GenericHandler<GetMetrics, Metric,
IdPolicy<agent_framework::model::enums::Component::Metric, NumberingZone::SHARED>>;

/*! @brief MetricHandler template specialization */
class MetricHandler : public MetricHandlerBase {
public:
    using MetricsArray = agent_framework::model::attribute::Array<agent_framework::model::Metric>;


    MetricHandler() : MetricHandlerBase() {}


    virtual ~MetricHandler();


    Array<SubcomponentEntry> fetch_sibling_uuid_list(Context& ctx,
                                                     const std::string& parent_uuid,
                                                     const std::string& collection_name) override {
        using CollectionName = agent_framework::model::enums::CollectionName;
        CollectionName metrics = CollectionName::Metrics;

        if (ctx.get_parent_component() != Component::None || !parent_uuid.empty()  ||
            collection_name != metrics.to_string()) {
            log_error("rest", std::string("Logic error. Tried to poll list of ") + collection_name + " under parent " +
                              + ctx.get_parent_component().to_string() + " with uuid " + parent_uuid + ".");
            return Array<SubcomponentEntry>();
        }

        try {
            log_debug("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Fetching list of all metrics from agent "
                << ctx.agent->get_gami_id());

            agent_framework::model::requests::GetMetrics get_all_metrics{};
            auto metrics_array = ctx.agent->execute<MetricsArray>(get_all_metrics);

            // Convert array of Metrics to Array of SubcomponentEntry
            Array<SubcomponentEntry> res;
            for (const auto& metric : metrics_array) {
                res.add_entry(metric.get_uuid());
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

    Metric fetch_entry(Context& ctx, const std::string& parent, const std::string& uuid) override {
        GetMetrics request{};
        request.set_metric(uuid);
        log_debug("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "Fetching [" << component_s() << " " << uuid << "]");
        try {
            auto metrics_array = ctx.agent->execute<MetricsArray>(request);
            if (metrics_array.size() == 0) {
                throw agent_framework::exceptions::InvalidUuid(
                    std::string(Metric::get_collection_name().to_string()) + " [UUID = '" + uuid + "'] not found.");
            }
            if (metrics_array.size() >= 2) {
                log_error("rest", std::string("Logic error. Found two or more metrics with uuid ") + uuid + ".");
            }
            auto element = metrics_array[0];
            element.set_parent_uuid(parent);
            element.set_uuid(uuid);
            element.set_parent_type(ctx.get_parent_component());
            return element;
        }
        catch (const json_rpc::JsonRpcException& e) {
            log_error("rest",
                      ctx.indent << "[" << char(ctx.mode) << "] "
                                 << "RPC Error while fetching [" << component_s() << " - "
                                 << uuid << "]: " << e.what());
            throw;
        }
        catch (const psme::core::agent::AgentUnreachable& e) {
            log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                     << "RPC Error (AgentUnreachable) while fetching ["
                                                     << component_s() << " " << uuid << "] " << e.what());
            throw;
        }
        catch (const psme::rest::error::ServerException& e) {
            log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                     << "Server Error while fetching ["
                                                     << component_s() << " " << uuid << "] "
                                                     << e.get_error().as_string());
            throw;
        }
        catch (...) {
            log_error("rest", "Other exception while fetching a " << component_s());
            throw;
        }
    }
};

MetricHandler::~MetricHandler() {}

}
}
}
}
