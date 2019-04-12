/*!
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
 *
 * @file registration_request.hpp
 *
 * @brief Registration request data
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"

#include <vector>
#include <string>



namespace agent_framework {
namespace generic {

/*! Agent group */
const char AGENT_TYPE[] = "Compute";
/*! Agent version */
const char AGENT_VERSION[] = "1.0.0";
/*! Agent vendor */
const char AGENT_VENDOR[] = "Intel Corporation";
/*! Agent RPC server IP */
const char AGENT_RPC_SERVER_IP[] = "localhost";
/*! Command server default port */
constexpr std::uint16_t DEFAULT_COMMAND_SERVER_PORT = 7777;

/*! Registration request data */
class RegistrationRequest {
public:
    /*! Construct registration request object */
    RegistrationRequest();


    /*!
     * @brief Capability attributes for agent
     */
    class Capability {
    public:
        /*!
         * @brief Create Capability object
         *
         * @param name agent name e.g. "compute"
         */
        explicit Capability(const std::string& name) : m_name{name} {}


        /*!
         * @brief Gets agent capability name
         *
         * @return name
         */
        const std::string& get_name() const {
            return m_name;
        }


    private:
        std::string m_name{};
    };

    /*! Capabilities array */
    using Capabilities = std::vector<Capability>;


    /*!
     * @brief Convert RegistrationRequest to JSON format
     *
     * @return JSON object
     */
    json::Json to_json();


private:
    /*!
    * @brief Load registration settings from configuration file
    */
    void load_configuration();


    /*!
     * @brief Return capabilities as JSON object
     *
     * @return JSON object
     */
    json::Json get_capabilities();


    std::string m_type{AGENT_TYPE};
    std::string m_version{AGENT_VERSION};
    std::string m_vendor{AGENT_VENDOR};
    std::string m_ipv4address{AGENT_RPC_SERVER_IP};
    std::uint16_t m_port{DEFAULT_COMMAND_SERVER_PORT};
    Capabilities m_capabilities{};
};
}
}

