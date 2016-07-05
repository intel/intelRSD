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
 * @file command/chassis/get_collection.hpp
 * @brief Generic chassis GetCollection command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_COLLECTION_HPP
#define AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_COLLECTION_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/array.hpp"
#include "agent-framework/module-ref/model/attributes/subcomponent_entry.hpp"

namespace agent_framework {
namespace command {
namespace chassis {

/* Forward declaration */
namespace json { class GetCollection; }

/*! Generic chassis command GetCollection */
class GetCollection : public Command {
public:
    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Chassis";

    /*! Tag string for identify GetCollection command */
    static constexpr const char TAG[] = "getCollection";

    /*! Argument request to execute */
    class Request : public Argument {
        friend class json::GetCollection;
        string m_component{};
        string m_name{};
    public:
        /*!
         * @brief Get component UUID
         * @return Component UUID
         * */
        const string& get_component() const {
            return m_component;
        }

        /*!
         * @brief Get collection name
         * @return Collection name
         * */
        const string& get_name() const {
            return m_name;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        using Subcomponents = model::attribute::Array<model::attribute::SubcomponentEntry>;


        /*!
         * Sets subcomponents
         * @param[in] subcomponents Subcomponents
         * */
        void set_subcomponents(const Subcomponents& subcomponents) {
            m_subcomponents = subcomponents;
        }

        /*! Default class constructor */
        Response() {}

        /*!
         * @brief Add subcomponent to response
         * @param[in] subcomponent Subcomponent
         * */
        void add_subcomponent(const model::attribute::SubcomponentEntry& subcomponent) {
            m_subcomponents.add_entry(subcomponent);
        }
    private:
        Subcomponents m_subcomponents{};
        friend class json::GetCollection;
    public:
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
    virtual ~GetCollection();

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

#endif /* AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_COLLECTION_HPP */
