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
 * */

#include "agent-framework/version.hpp"



using namespace agent_framework::generic;

constexpr const char* Version::BUILD_TIME;
constexpr const char* Version::VERSION_STRING;
constexpr int Version::MAJOR_INDEX;
constexpr int Version::MINOR_INDEX;
constexpr int Version::BUILD_INDEX;

namespace {
const char SEP = '.';
}


std::string Version::build_info() {
    std::string build_prefix{};
    switch (Version::BUILD_TYPE) {
        case ENG:
            build_prefix = "ENGINEERING PSME BUILD " + std::string(Version::VERSION_STRING);
            break;
        case REL:
            build_prefix = "RELEASE PSME BUILD " + std::string(Version::VERSION_STRING);
            break;
            // treating MAN case the same as default
        case MAN:
        default:
            build_prefix = "MANUAL PSME BUILD";
            break;
    }
    return build_prefix + "; Built " + std::string(Version::BUILD_TIME);
}


int agent_framework::generic::get_version_component(const char* version_string, int index) {
    if (nullptr == version_string) {
        return 0;
    }

    //return 0 minor version for get_device_id ipmb command
    if (index == Version::MINOR_INDEX) {
        return 0;
    }
    else {
        const char *s = version_string;

        int i = 0;
        while (i < index && *s) {
            if (SEP == *(s++)) {
                ++i;
            }
        }
        if (i != index) {
            return 0;
        }

        int value{0};
        while (*s) {
            if (isdigit(*s)) {
                value = (value * 10) + (*s - '0');
            }
            else if (SEP == *s) {
                break;
            }
            else {
                return 0;
            }
            ++s;
        }
        return value;
    }
}
