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
 * @file command/storage/set_component_attributes.hpp
 * @brief Generic SetComponentAttributes command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_STORAGE_SET_COMPONENT_ATTRIBUTES_HPP
#define AGENT_FRAMEWORK_COMMAND_STORAGE_SET_COMPONENT_ATTRIBUTES_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/attributes.hpp"

#include <jsonrpccpp/server.h>

#include <vector>

namespace agent_framework {
namespace command {
namespace storage {

/* Forward declaration */
namespace json { class SetComponentAttributes; }

/*! Generic command SetComponentAttributes */
class SetComponentAttributes : public Command {
public:
    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Storage";

    /*! Tag string for identify SetComponentAttributes command */
    static constexpr const char TAG[] = "setComponentAttributes";

    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::SetComponentAttributes;
        string m_component{};
        model::attribute::Attributes m_attributes{};
    public:
        /*!
         * @brief Get component UUID
         * @return Component UUID
         * */
        const string& get_component() const {
            return m_component;
        }

        /*!
         * @brief Get attributes
         * @return Attributes
         * */
        const model::attribute::Attributes& get_attributes() const {
            return m_attributes;
        }

        ~Request();

    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:

        /*! Default class constructor */
        Response() {}
        ~Response();
    };

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~SetComponentAttributes();

protected:

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   in          Input request argument
     * @param[out]  out         Output response argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_STORAGE_SET_COMPONENT_ATTRIBUTES_HPP */
