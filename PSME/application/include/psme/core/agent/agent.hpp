/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file agent.hpp
 *
 * @brief Agent base class implementation
 * */

#pragma once
#include "capabilities.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace psme {
namespace core {
/*! Register Agent */
namespace agent {

// #<{(|! Forward declaration |)}>#
// class Invoker;

/*! Base Agent class */
class Agent {
public:
    /*! Agent constructor */
    Agent() { }

    /*! Agent constructor */
    Agent(const std::string& gami_id, const std::string& ipv4address, const int port)
     : m_gami_id(gami_id), m_ipv4address(ipv4address), m_port(port) { }

    /*! Agent destructor */
    virtual ~Agent();

    /*!
     * @brief Get agent gami id from request
     * @return agent gami id
     */
    const std::string& get_gami_id() const {
        return m_gami_id;
    }

    /*!
    * @brief Get agent version from request
    * @return agent version
    * */
    const std::string& get_version() const {
        return m_version;
    }

    /*!
    * @brief Get agent`s RPC Server IPv4 address from request
    * @return IPv4 address
    * */
    const std::string& get_ipv4address() const {
        return m_ipv4address;
    }

    /*!
    * @brief Get agent`s RPC Server TCP port from request
    * @return access port
    * */
    const int& get_port() const {
        return m_port;
    }

    /*!
    * @brief Get agent vendor from request
    * @return agent vendor
    * */
    const std::string& get_vendor() const {
        return m_vendor;
    }

    /*!
     * @brief Get agent capabilites from request
     *
     * @return agent capabilities
     */
    const Capabilities& get_capabilities() const {
        return m_capabilities;
    }

    /*!
     * @brief Check if agent has specified capability
     *
     * @param name capability name
     *
     * @return On successed true, otherwise false
     */
    bool has_capability(const std::string& name) {
        return std::any_of( m_capabilities.cbegin(),
                            m_capabilities.cend(),
                            [&name](const Capability& capability) {
                                return capability.get_name() == name;
                            });
    }

    // #<{(|!
    //  * @brief Get agent invoker object
    //  * @return Invoker object
    //  |)}>#
    // virtual Invoker& get_invoker() = 0;

protected:
    /*! agent id */
    std::string m_gami_id{};
    /*! agent IPv4 address */
    std::string m_ipv4address{};
    /*! agent port */
    int m_port{};

    /*! agent API version */
    std::string m_version{};
    /*! agent vendor name */
    std::string m_vendor{};

    /*! agent capabilites */
    Capabilities m_capabilities{};
};

using AgentSharedPtr = std::shared_ptr<Agent>;
using AgentUniquePtr = std::unique_ptr<Agent>;

}
}
}
