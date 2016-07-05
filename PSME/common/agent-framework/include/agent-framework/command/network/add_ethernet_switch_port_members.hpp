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
 * @file command/network/add_ethernet_switch_port_members.hpp
 * @brief Generic network AddEthernetSwitchPortMembers command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_MEMBERS_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_MEMBERS_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/oem.hpp"
#include "agent-framework/module-ref/types/common.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json { class AddEthernetSwitchPortMembers; }

using MemberArray = std::vector<std::string>;

/*! Generic network command AddEthernetSwitchPortMembers */
class AddEthernetSwitchPortMembers : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "addEthernetSwitchPortMembers";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~AddEthernetSwitchPortMembers();
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
        friend class json::AddEthernetSwitchPortMembers;
        MemberArray m_members{};
        std::string m_port{};
        agent_framework::model::attribute::Oem m_oem{};
    public:
        /*!
         * @brief Get port UUID from request
         * @return UUID string
         * */
        const std::string& get_port() const {
            return m_port;
        }

        /*!
         * @brief Get OEM specific data
         * @return AddEthernetSwitchPortMembers OEM specific data
         * */
        const agent_framework::model::attribute::Oem& get_oem() const {
            return m_oem;
        }

        /*!
         * @brief The UUIDs of the members (physical switch ports).
         * @return Reference to vector of member ports.
         * */
        const MemberArray& get_members() const {
            return m_members;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::AddEthernetSwitchPortMembers;
        agent_framework::model::attribute::Oem m_oem{};

    public:
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

#endif
/* AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_ETHERNET_SWITCH_PORT_MEMBERS_HPP */
