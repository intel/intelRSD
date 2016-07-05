/*!
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
 *
 * @file command/network/add_ethernet_switch_port.hpp
 * @brief Generic network AddEthernetSwitchPort command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/enum/network.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json {
    class AddEthernetSwitchPort;
}

using MembersArray = std::vector<std::string>;

/*! Generic network command AddEthernetSwitchPort */
class AddEthernetSwitchPort : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "addEthernetSwitchPort";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~AddEthernetSwitchPort();
protected:
    /*!
     * @brief Execute command with given command arguments
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
    private:
        friend class json::AddEthernetSwitchPort;
        std::string m_switch{};
        std::string m_port_identifier{};
        model::enums::PortMode m_mode{model::enums::PortMode::Unknown};
        std::vector<std::string> m_members{};
        agent_framework::model::attribute::Oem m_oem{};
    public:
        /*!
         * @brief Get switch UUID from request
         * @return UUID string
         * */
        const std::string& get_switch() const {
            return m_switch;
        }

        /*!
         * @brief Get port identifier from request
         * @return port identifier string
         * */
        const std::string& get_port_identifier() const {
            return m_port_identifier;
        }

        /*!
         * @brief Get mode from request
         * @return mode string
         * */
        const agent_framework::model::enums::PortMode& get_mode() const {
            return m_mode;
        }

        /*!
         * @brief The UUIDs of the members.
         * @return Reference to vector of member ports.
         * */
        const std::vector<std::string>& get_members() const {
            return m_members;
        }

        /*!
         * @brief Get OEM specific data
         * @return AddEthernetSwitchPort OEM specific data
         */
        const agent_framework::model::attribute::Oem& get_oem() {
            return m_oem;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::AddEthernetSwitchPort;
        std::string m_port{};
        agent_framework::model::attribute::Oem m_oem{};

    public:
        /*!
         * @brief Set port UUID
         * @param[in] port Port UUID
         * */
        void set_port(const std::string& port) {
            m_port = port;
        }

        /*!
         * @brief Set OEM data
         * @param[in] oem OEM data
         */
        void set_oem(const agent_framework::model::attribute::Oem& oem) {
            m_oem = oem;
        }

        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_HPP */
