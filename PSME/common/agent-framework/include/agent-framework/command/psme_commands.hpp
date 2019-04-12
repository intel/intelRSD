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
 * @file psme_commands.hpp
 *
 * @brief Declarations of all psme commands
 * */

#pragma once
#include "agent-framework/command/command.hpp"
#include "agent-framework/module/requests/psme/heart_beat.hpp"
#include "agent-framework/module/requests/psme/attach.hpp"
#include "agent-framework/module/requests/psme/component_notification.hpp"
#include "agent-framework/module/responses/psme/heart_beat.hpp"
#include "agent-framework/module/responses/psme/attach.hpp"
#include "agent-framework/module/responses/psme/component_notification.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"

namespace agent_framework {
namespace command {

// eventing: ComponentNotification is a notification, so it doesn't actually need a response
using ComponentNotification = Command<model::requests::ComponentNotification, model::responses::ComponentNotification>;

// registration
using Attach = Command<model::requests::Attach, model::responses::Attach>;

// heart beat
using HeartBeat = Command<model::requests::HeartBeat, model::responses::HeartBeat>;

}
}
