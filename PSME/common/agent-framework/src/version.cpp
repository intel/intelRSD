/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#include "agent-framework/version.hpp"

using namespace agent_framework::generic;

std::string Version::to_string() {
    return std::to_string(agent_framework::generic::Version::MAJOR) + "." +
           std::to_string(agent_framework::generic::Version::MINOR) + "." +
           std::to_string(agent_framework::generic::Version::BUILD) + "." +
           std::to_string(agent_framework::generic::Version::HOTFIX);
}

std::string Version::build_info() {
    std::string build_prefix;
    switch(Version::BUILD_TYPE) {
        case ENG:
                build_prefix = "ENGINEERING PSME BUILD " +
                    std::to_string(agent_framework::generic::Version::MAJOR)
                    + "." +
                    std::to_string(agent_framework::generic::Version::MINOR)
                    + "." +
                    std::to_string(agent_framework::generic::Version::BUILD);
            break;
        case REL:
                build_prefix = "RELEASE PSME BUILD " + to_string();
            break;
        // treating MAN case the same as default
        case MAN:
        default:
                build_prefix = "MANUAL PSME BUILD";
            break;
    }
    return build_prefix + "; Built " + std::string(Version::BUILD_TIME);
}
