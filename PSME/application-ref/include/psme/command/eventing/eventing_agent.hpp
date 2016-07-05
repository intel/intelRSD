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
 * @file command/eventing/eventing_agent.hpp
 *
 * @brief Register Agent command.
 * */

#ifndef PSME_COMMAND_EVENTING_EVENTING_AGENT_HPP
#define PSME_COMMAND_EVENTING_EVENTING_AGENT_HPP

#include "psme/command/eventing/tag.hpp"
#include "psme/command/command.hpp"

#include <string>

namespace psme {
namespace command {
namespace eventing {

/* Forward declaration */
namespace json { class EventingAgent; }

using std::string;

/*! Command EventingAgent */
class EventingAgent : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify EventingAgent command */
    static const char TAG[];

    /*!
     * @brief Create generic command EventingAgent for eventing server
     *
     * @param[in]   implementation  Set user tag string to identify particular
     *                              command implementation
     * */
    EventingAgent(const string& implementation)
        : Command(implementation, eventing::TAG, EventingAgent::TAG) { }

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~EventingAgent();
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
        friend class json::EventingAgent;
        std::string m_gami_id{};
        std::string m_component{};
        std::string m_notification{};
        std::string m_type{};
        std::string m_parent{};

    public:
        /*!
        * @brief Gets gami id
        *
        * @return gami id
        */
        const std::string& get_gami_id() const {
            return m_gami_id;
        }

         /*!
        * @brief Gets component id
        *
        * @return component id
        */
        const std::string& get_component() const {
           return m_component;
        }

       /*!
        * @brief Gets component type
        *
        * @return component type
        */
        const std::string& get_type() const {
           return m_type;
        }

        const std::string& get_notification() const {
            return m_notification;
        }

        const std::string& get_parent() const {
            return m_parent;
        }

        /*! Request default constructor */
        Request() = default;
        /*! Request default copy constructor */
        Request(const Request&) = default;
        /*! Request default assigment operator */
        Request& operator=(const Request&) = default;
        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    private:
        friend class json::EventingAgent;
    public:
        ~Response();
    };

};

}
}
}

#endif /* PSME_COMMAND_COMPUTE_GET_MEMORY_INFORMATION_HPP */
