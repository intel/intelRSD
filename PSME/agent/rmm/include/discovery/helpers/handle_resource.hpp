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
 * @file rmm/discovery/helpers/handle_resources.hpp
 */

#pragma once



#include "discovery/helpers/common.hpp"
#include "discovery/helpers/update.hpp"
#include "discovery/helpers/discover.hpp"
#include "discovery/helpers/log_notify.hpp"
#include "discovery/helpers/get_resources.hpp"
#include "discovery/helpers/metrics.hpp"



namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

/*!
 * Generic resource discovery handler. It is used to to perform discovery of various resources. It requires four
 * arguments: discovery context used for discovery, discovery params that store additional parameters,
 * parent uuid and current presence of the resource.
 * */
template<RmmType TYPE>
void handle_resource(const DiscoveryContext& dc, const DiscoveryParams<TYPE>& dp, const std::string& parent,
                     bool is_present) {

    auto component = get_model_component<TYPE>();
    auto resources = get_resources<TYPE>(dp, parent);

    switch (get_action_type(resources.size() == 1, is_present)) {
        case DiscoveryAction::Add: {
            try {
                auto resource = discover<TYPE>(parent, dc, dp);
                update<TYPE>(resource, dc, dp);
                const auto stable_uuid = log_notify_and_add<TYPE>(resource, dc.event_collector);
                build_metrics<TYPE>(stable_uuid, dc, dp);

                // read metrics for the first time
                auto stable_resource = module::get_manager<ModelType<TYPE>>().get_entry(stable_uuid);
                bool resource_changed = update_metrics<TYPE>(stable_resource, dc, dp);
                log_notify_and_update<TYPE>(stable_resource, dc.event_collector, resource_changed);
            }
            catch (std::exception& e) {
                log_error("rmm-discovery", "Unable to add " << component.to_string() << ": " << e.what());
            }
            break;
        }
        case DiscoveryAction::Update: {
            try {
                auto resource = module::get_manager<ModelType<TYPE>>().get_entry(resources.front());
                bool was_changed = update<TYPE>(resource, dc, dp) || update_metrics<TYPE>(resource, dc, dp);
                log_notify_and_update<TYPE>(resource, dc.event_collector, was_changed);
            }
            catch (std::exception& e) {
                log_error("rmm-discovery", "Unable to update " << component.to_string() << " (" << resources.front()
                    << "): " << e.what());
            }
            break;
        }
        case DiscoveryAction::Remove:
            remove_metrics<TYPE>(dc, resources.front());
            log_notify_and_remove<TYPE>(resources.front(), dc.event_collector);
            break;
        case DiscoveryAction::None:
        default:
            break;
    }
}

}
}
}
}
