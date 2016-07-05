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
 * @file command/compute/get_chassis_info.hpp
 * @brief Generic chassis GetChassisInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_CHASSIS_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_CHASSIS_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"

namespace agent_framework {
namespace command {
namespace compute {

/* Forward declaration */
namespace json { class GetChassisInfo; }

/*! Generic compute command GetChassisInfo */
class GetChassisInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Compute";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "getChassisInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetChassisInfo();
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
        friend class json::GetChassisInfo;
        string m_chassis{};
    public:
        /*!
         * @brief Get chassis UUID from request
         * @return uuid string
         * */
        const string& get_chassis() const {
            return m_chassis;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        /*!
         * Sets chassis
         * @param[in] chassis Chassis
         * */
        void set_chassis(const model::Chassis& chassis) {
            m_chassis = chassis;
        }

        ~Response();

    private:
        friend class json::GetChassisInfo;
        model::Chassis m_chassis{};
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_CHASSIS_INFO_HPP */
