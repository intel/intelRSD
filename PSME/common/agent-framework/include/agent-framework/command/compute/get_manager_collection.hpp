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
 * @file command/compute/get_manager_collection.hpp
 * @brief Generic compute GetManagerCollection command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MANAGER_COLLECTION_HPP
#define AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MANAGER_COLLECTION_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/attributes/array.hpp"
#include "agent-framework/module-ref/model/attributes/manager_entry.hpp"

namespace agent_framework {
namespace command {
namespace compute {

/* Forward declaration */
namespace json { class GetManagerCollection; }

/*! Generic compute command GetCollection */
class GetManagerCollection : public Command {
public:
    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Compute";

    /*! Tag string for identify GetCollection command */
    static constexpr const char TAG[] = "getManagerCollection";

    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::GetManagerCollection;
    public:
        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::GetManagerCollection;
        using Managers = model::attribute::Array
            <model::attribute::ManagerEntry>;
        Managers m_managers{};
    public:
        /*!
         * Sets Managers
         * @param[in] managers Managers
         * */
        void set_managers(const Managers& managers) {
            m_managers = managers;
        }

        /*!
         * Adds Manager
         * @param[in] manager Manager
         * */
        void add_manager(const model::attribute::ManagerEntry& manager) {
            m_managers.add_entry(manager);
        }

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
    virtual ~GetManagerCollection();

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

#endif /* AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MANAGER_COLLECTION_HPP */
