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
 * @file command/registration/register_agent.hpp
 *
 * @brief Register Agent command.
 * */

#pragma once
#include "psme/core/agent/capabilities.hpp"
#include "psme/command/registration/tag.hpp"
#include "psme/command/command.hpp"

#include <string>

namespace psme {
namespace command {
namespace registration {

/* Forward declaration */
namespace json { class RegisterAgent; }

using std::string;

/*! Command RegisterAgent */
class RegisterAgent : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify RegisterAgent command */
    static const char* TAG;

    /*!
     * @brief Create generic command RegisterAgent for registration server
     *
     * @param[in]   implementation  Set user tag string to identify particular
     *                              command implementation
     * */
    RegisterAgent(const string& implementation)
        : Command(implementation, registration::TAG, RegisterAgent::TAG) { }

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~RegisterAgent();
protected:
    /*!
     * @brief Execute command with givent command arguments
     *
     * @param[in]   in      Input command argument
     * @param[out]  out     Output command argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
public:
    /*! Argument request to execute */
    class Request : public Argument {
    public:

        /*!
         * @brief Get agent identifier from request
         * @return agent identifier
         * */
        const std::string& get_gami_id() const {
            return m_gami_id;
        }

        /*!
         * @brief Get agent version from request
         * @return agent version
         * */
        const string& get_version() const {
            return m_version;
        }
        /*!
         * @brief Get agent`s RPC Server IPv4 address from request
         * @return IPv4 address
         * */
        const string& get_ipv4address() const {
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
        const string& get_vendor() const {
            return m_vendor;
        }

        /*!
         * @brief Get agent capabilities from request
         *
         * @return agent capabilities
         */
        const psme::core::agent::Capabilities& get_capabilities() const {
            return m_capabilities;
        }

        /*!
         * @brief Check is request is valid
         * @return On successed true is returned, otherwise false
         */
        bool is_valid() const {
            if (m_gami_id.empty() ||
                m_capabilities.size() <= 0 ||
                m_ipv4address.empty() ||
                m_version.empty() ||
                m_vendor.empty() ||
                m_port <= 0) {
                return false;
            }
            return true;
        }

        ~Request();

    private:
        friend class json::RegisterAgent;
        string m_gami_id{};
        string m_version{};
        string m_vendor{};
        string m_ipv4address{};
        int m_port{};
        psme::core::agent::Capabilities m_capabilities{};
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        ~Response();

        /*!
         * @brief Get IPv4 address of communication thread assigned to agent.
         * @return IPv4 address of communication thread assigned to agent.
         * */
        const std::string& get_ipv4address() const {
            return m_ipv4address;
        }

        /*!
         * @brief Set to response IPv4 address of communication
         * thread assigned to agent.
         * */
        inline void set_ipv4address(const string& ipv4address) {
            m_ipv4address = ipv4address;
        }

        /*!
         * @brief Get port number of communication thread assigned to agent.
         * @return Port number of communication thread assigned to agent.
         * */
        int get_port() const {
            return m_port;
        }

        /*!
         * @brief Set to response TCP Port of communication
         * thread assigned to agent.
         * */
        inline void set_port(const int& port) {
            m_port = port;
        }

        /*!
         * @brief Get protocol version application is compatible with.
         * @return Protocol version application is compatible with.
         * */
        const std::string& get_version() const {
            return m_version;
        }

        /*!
         * @brief Set to response version of Application
         * */
        inline void set_version(const string& version) {
            m_version = version;
        }

    private:
        std::string m_ipv4address{};
        std::string m_version{};
        int m_port{};
    };

};

}
}
}

