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
 * @file command/network/get_ethernet_switch_info.hpp
 * @brief Generic network GetEthernetSwitchInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/switch.hpp"

namespace agent_framework {
namespace command {
namespace network {

/* Forward declaration */
namespace json {
    class GetEthernetSwitchInfo;
}

/*! @brief Generic network command GetEthernetSwitchInfoInfo */
class GetEthernetSwitchInfo : public Command {
public:
    class Request;
    class Response;

    /*! @brief Tag string for identify agent */
    static constexpr const char AGENT[] = "Network";

    /*! @brief Tag string for identify command */
    static constexpr const char TAG[] = "getEthernetSwitchInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! @brief Command destructor */
    virtual ~GetEthernetSwitchInfo();
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
    /*! @brief Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::GetEthernetSwitchInfo;
        std::string m_switch{};
    public:
        /*!
         * @brief Get switch from request
         * @return UUID string
         * */
        const string& get_switch() const {
            return m_switch;
        }

        /*! @brief Destructor */
        ~Request();
    };

    /*! @brief Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::GetEthernetSwitchInfo;
        agent_framework::model::Switch m_switch{};
    public:
        /*!
         * @brief Set Switch module
         * @param switch_module Switch module reference
         */
        void set_switch(const agent_framework::model::Switch& switch_module) {
            m_switch = switch_module;
        }

        /*! @brief Destructor */
        ~Response();
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_NETWORK_GET_ETHERNET_SWITCH_INFO_HPP */
