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
 * @file command/network/add_vlan.hpp
 * @brief Generic network AddVlan command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_VLAN_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_VLAN_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json {
    class AddVlan;
}

/*! Generic network command AddVlan */
class AddVlan : public Command {
public:
    class Request;
    class Response;

    /*! Shortcut for Oem Type */
    using Oem = agent_framework::model::attribute::Oem;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "addVlan";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~AddVlan();
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
        friend class json::AddVlan;
        std::string m_port{};
        std::string m_vlan_name{};
        unsigned int m_vlan_id{};
    public:
        /*!
         * @brief Get port UUID
         * @return UUID as std::string
         * */
        const string& get_port() const {
            return m_port;
        }

        /*!
         * @brief Get vlan id from request
         * @return vlan id
         */
        unsigned int get_vlan_id() const {
            return m_vlan_id;
        }

        /*!
         * @brief Get vlan name
         * @return Vlan name
         * */
        const string& get_vlan_name() const {
            return m_vlan_name;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::AddVlan;
        std::string m_vlan{};
        Oem m_oem{};

    public:
        /*!
         * @brief Set port VLAN UUID
         * @param[in] vlan  VLAN UUID
         * */
        void set_vlan(const std::string& vlan) {
            m_vlan = vlan;
        }

        /*!
         * @brief Set OEM data
         * @param oem data
         */
        void set_oem(const Oem& oem) {
            m_oem = oem;
        }

        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_ADD_VLAN_HPP */
