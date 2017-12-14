/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file rmm/default_configuration.hpp
 */

#pragma once

namespace agent {
namespace generic {

/*!
 * @brief Default configuration for the agent
 *
 * Logger agent - main logger in AGENT_FRAMEWORK Agent
 * */
static constexpr const char DEFAULT_CONFIGURATION[] = R"({
"agent":{"vendor" : "Intel Corporation","capabilities":["Rmm"]},
"server":{"port":7791},
"registration":{"ipv4":"localhost","port":8383,"interval":3},
"database": { "location": "/var/opt/psme/rmm" },
"logger":{"agent":{}},
"managers": [
    {"locationOffset": 0, "device": "/dev/ttyCm1IPMI"},
    {"locationOffset": 4, "device": "/dev/ttyCm2IPMI"}
]
})";

/*! @brief Default environment variable for configuration file */
static constexpr const char DEFAULT_ENV_FILE[] =
    "AGENT_RMM_CONFIGURATION_FILE";

    static constexpr char DEFAULT_VALIDATOR_JSON[] = R"({
    "database": {
        "location": {
            "validator": true,
            "type": "string"
        }
    },
    "logger" : {
        "agent" : {
            "level" : {
                "validator" : true,
                "type" : "string",
                "anyof": ["EMERGENCY",
                          "ALERT",
                          "CRITICAL",
                          "ERROR",
                          "WARNING",
                          "NOTICE",
                          "INFO",
                          "DEBUG"]
            }
        }
    }
    })";

/*!
 * @brief Default configuration file to load when environment
 * isn't present or valid.
 * */
static constexpr const char DEFAULT_FILE[] = "configuration.json";

}
}
