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
 * @file command/chassis/get_fan_info.hpp
 * @brief Generic chassis GetFanInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_FAN_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_FAN_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/fan.hpp"
#include "agent-framework/module-ref/constants/common.hpp"

namespace agent_framework {
namespace command {
namespace chassis {

/* Forward declaration */
namespace json { class GetFanInfo; }

/*! Generic chassis command GetFanInfo */
class GetFanInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Chassis";

    /*! Tag string for identify GetFanInfo command */
    static constexpr const char TAG[] = "getFanInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetFanInfo();
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
        friend class json::GetFanInfo;
        string m_component{};
    public:
        /*!
         * @brief Get component id from request
         * @return Computer system uuid
         * */
        const string& get_component() const {
            return m_component;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        /*!
         * Sets fan
         * @param[in] fan Fan
         * */
        void set_fan(const model::Fan& fan) {
            m_fan = fan;
        }

        ~Response();
    private:
        friend class json::GetFanInfo;
        model::Fan m_fan{};
    };
};

}
}
}

#endif	/* AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_FAN_INFO_HPP */
