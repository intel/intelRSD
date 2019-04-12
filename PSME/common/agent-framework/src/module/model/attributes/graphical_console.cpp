/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/graphical_console.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

json::Json GraphicalConsole::to_json() const {
    json::Json json = json::Json();
    json[literals::GraphicalConsole::ENABLED] = get_enabled();
    json[literals::GraphicalConsole::MAX_SESSIONS] = get_max_sessions();
    json[literals::GraphicalConsole::TYPES_SUPPORTED] = get_types_supported().to_json();
    return json;
}

GraphicalConsole GraphicalConsole::from_json(const json::Json& json) {
    GraphicalConsole console;
    console.set_enabled(json[literals::GraphicalConsole::ENABLED]);
    console.set_max_sessions(json[literals::GraphicalConsole::MAX_SESSIONS]);
    console.set_types_supported(SupportedConnections::from_json(
        json[literals::GraphicalConsole::TYPES_SUPPORTED]));
    return console;
}
