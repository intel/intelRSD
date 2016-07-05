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
 * @file command/chassis/get_authorization_certificate.hpp
 * @brief Generic chassis GetAuthorizationCertificate command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_AUTHORIZATION_CERTIFICATE_HPP
#define AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_AUTHORIZATION_CERTIFICATE_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/authorization_certificate.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"

namespace agent_framework {
namespace command {
namespace chassis {

/* Forward declaration */
namespace json { class GetAuthorizationCertificate; }

/*! Generic chassis command GetAuthorizationCertificate */
class GetAuthorizationCertificate : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Chassis";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "getAuthorizationCertificate";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetAuthorizationCertificate();
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
        friend class json::GetAuthorizationCertificate;
        std::string m_certificate{};
    public:
        /*!
         * @brief Get certificate type from request
         * @return Type of certifciate ("PODM", "RMM")
         * */
        const std::string& get_certificate() const {
            return m_certificate;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:

        /*! Default constructor */
        Response() { }

        /*!
         * Sets certificate
         * @param[in] certificate Certificate
         * */
        void set_certificate(const  model::AuthorizationCertificate& certificate) {
            m_certificate = certificate;
        }

        ~Response();

    private:
        friend class json::GetAuthorizationCertificate;
        model::AuthorizationCertificate m_certificate{};
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_CHASSIS_GET_Authorization_CERTIFICATE_HPP */
