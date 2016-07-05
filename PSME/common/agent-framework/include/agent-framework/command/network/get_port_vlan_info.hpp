/*!
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command/network/get_port_vlan_info.hpp
 * @brief Generic network GetPortVlanInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_GET_PORT_VLAN_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_GET_PORT_VLAN_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/port_vlan.hpp"
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json { class GetPortVlanInfo; }

/*! Generic network command GetPortVlanInfo */
class GetPortVlanInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "getPortVlanInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetPortVlanInfo();
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
        friend class json::GetPortVlanInfo;
        std::string m_port_vlan{};
    public:
        /*!
         * @brief Get port vlan uuid
         * @return Port vlan uuid
         * */
        const string& get_port_vlan() const {
            return m_port_vlan;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::GetPortVlanInfo;
        model::PortVlan m_port_vlan{};
    public:
        /*!
         * @brief Set switch port vlan info
         * @param port_vlan PortVlan class reference
         */
        void set_port_vlan(const model::PortVlan& port_vlan) {
            m_port_vlan = port_vlan;
        }

        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_GET_PORT_VLAN_INFO_HPP */
