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
 * @file command/network/get_ethernet_switch_port_info.hpp
 * @brief Generic network GetEthernetSwitchPortInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_PORT_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_PORT_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/switch_port.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json {
    class GetEthernetSwitchPortInfo;
}

/*! Generic network command GetEthernetSwitchInfoInfo */
class GetEthernetSwitchPortInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "getEthernetSwitchPortInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetEthernetSwitchPortInfo();
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
    private:
        friend class json::GetEthernetSwitchPortInfo;
        std::string m_port{};
    public:
        /*!
         * @brief Get port UUID
         * @return uuid string
         * */
        const string& get_port() const {
            return m_port;
        }

        /*! @brief Destructor */
        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::GetEthernetSwitchPortInfo;
        agent_framework::model::SwitchPort m_port{};
    public:
        /*!
         * @brief Set Port module
         * @param port Switch module reference
         */
        void set_port(const agent_framework::model::SwitchPort& port) {
            m_port = port;
        }

        /*! @brief Destructor */
        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_PORT_INFO_HPP */
