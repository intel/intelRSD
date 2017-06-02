/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 *
 * @file default_configuration.hpp
 *
 * @brief Application default configuration
 * */

#pragma once

namespace agent {
namespace generic {

/*!
 * @brief Default configuration for the agent
 *
 * Logger agent - main logger in AGENT_FRAMEWORK Agent
 * */
static constexpr const char DEFAULT_CONFIGURATION[] = R"({
"agent":{"vendor" : "Intel Corporation","capabilities":["Pnc"]},
"server":{"port":7781},
"registration":{"ipv4":"localhost","port":8383,"interval":3},
"logger":{"agent":{}},
"managers":[]
})";

/*! @brief Default environment variable for configuration file */
static constexpr const char DEFAULT_ENV_FILE[] =
    "AGENT_PNC_CONFIGURATION_FILE";

static constexpr char DEFAULT_VALIDATOR_JSON[] = R"({
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
