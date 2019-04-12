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
 * @file rmm/discovery/helpers/log_notify.hpp
 */

#pragma once

#include "discovery/helpers/common.hpp"
#include "discovery/helpers/stabilize.hpp"

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

/*!
 * This is a helper function that adds the resource to the model, logs it and adds add event to the event pool. Last
 * parameter is optional and is used to turn off (if false is passed) uuid stabilization, eventing (default is true).
 * Returns uuid of the resource (may change due to tree stability);
 */
template <RmmType TYPE>
std::string log_notify_and_add(const ModelType<TYPE>& resource, event_collector::EventCollectorInterfacePtr ec,
                               bool stabilize_notify_flag = true) {
    auto component = get_model_component<TYPE>();
    std::string uuid = resource.get_uuid();
    log_info("rmm-discovery", component.to_string() << " found");
    log_debug("rmm-discovery", component.to_string() << " uuid: " << uuid);
    module::get_manager<ModelType<TYPE>>().add_entry(resource);
    if (stabilize_notify_flag) {
        uuid = stabilize<TYPE>(uuid);
        ec->poll_add_event(component, uuid, resource.get_parent_uuid());
    }
    return uuid;
}

/*!
 * This is a helper function that removes the resource from the model, logs it and adds remove event to the event pool.
 */
template <RmmType TYPE>
void log_notify_and_remove(const std::string& uuid, event_collector::EventCollectorInterfacePtr ec) {
    auto component = get_model_component<TYPE>();
    log_info("rmm-discovery", component.to_string() << " removed");
    log_debug("rmm-discovery", "Removed " << component.to_string() << " uuid: " << uuid);
    std::string parent_uuid = module::get_manager<ModelType<TYPE>>().get_entry(uuid).get_parent_uuid();
    module::get_manager<ModelType<TYPE>>().remove_entry(uuid);
    ec->poll_remove_event(component, uuid, parent_uuid);
}

/*!
 * This is a helper function that updates the resource in the model, logs it and adds update event to the event pool.
 * Last parameter should be true if any changes were made. This is to remove many checks from the code.
 */
template <RmmType TYPE>
void log_notify_and_update(const ModelType<TYPE>& resource, event_collector::EventCollectorInterfacePtr ec,
                           bool was_changed) {
    // this could be done outside, but it simplifies code a bit.
    if (!was_changed) {
        return;
    }
    std::string uuid = resource.get_uuid();
    std::string parent_uuid = resource.get_parent_uuid();
    auto component = get_model_component<TYPE>();
    log_info("rmm-discovery", component.to_string() << " updated");
    log_debug("rmm-discovery", component.to_string() << " with uuid: " + uuid << " has been updated");
    module::get_manager<ModelType<TYPE>>().get_entry_reference(resource.get_uuid()).get_raw_ref() = resource;
    ec->poll_update_event(component, uuid, parent_uuid);
}

}
}
}
}
