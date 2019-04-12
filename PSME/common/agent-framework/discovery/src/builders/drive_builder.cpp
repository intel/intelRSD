/*!
 * @brief Drive builder class implementation.
 *
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
 * @file drive_builder.cpp
 */

#include "agent-framework/discovery/builders/drive_builder.hpp"


using namespace agent_framework::discovery;
using namespace agent_framework;


model::Drive DriveBuilder::build_default(const Uuid& parent_uuid) {
    model::Drive drive{parent_uuid};
    drive.set_status({model::enums::State::Enabled, model::enums::Health::OK});
    return drive;
}

void DriveBuilder::update_critical_state(agent_framework::model::Drive& drive) {
    drive.set_status(model::attribute::Status{
        model::enums::State::Disabled,
        model::enums::Health::Critical
    });
}
