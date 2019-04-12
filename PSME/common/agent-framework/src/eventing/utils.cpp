/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file utils.cpp
 * @brief Eventing utils
 * */

#include "agent-framework/eventing/utils.hpp"

using namespace agent_framework;

void eventing::send_event(const Uuid& uuid,
                          const model::enums::Component& component,
                          const model::enums::Notification& notification,
                          const Uuid& parent_uuid) {

    model::attribute::EventData edat{};
    edat.set_component(uuid);
    edat.set_type(component);
    edat.set_notification(notification);
    edat.set_parent(parent_uuid);
    eventing::EventsQueue::get_instance()->push_back(edat);
}
