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
 * @file error_factory.hpp
 *
 * @brief Declaration of ErrorFactory class
 * */
#ifndef PSME_ERROR_ERROR_FACTORY_HPP
#define PSME_ERROR_ERROR_FACTORY_HPP
#include "psme/rest-ref/server/error/server_exception.hpp"

namespace psme {
namespace rest {
namespace error {

/*!
 * @brief ServerError factory.
 *
 * Creates SeverError objects.
 * */
class ErrorFactory {
public:
    /*!
     * @brief Create ServerError for malformed uri.
     * @param[in] message Optional extended message.
     * @return Malformed uri ServerError
     * */
    static ServerError create_malformed_uri_error(const std::string& message = {});

    /*!
     * @brief Create ServerError for not existing resource.
     * @param[in] message Optional extended message.
     * @return Not existing resource ServerError
     * */
    static ServerError create_not_found_error(const std::string& message = {});

    /*!
     * @brief Create ServerError for method not allowed.
     *
     * @return Method not allowed ServerError
     * */
    static ServerError create_method_not_allowed_error();

    /*!
     * @brief Create ServerError for unauthorized access.
     *
     * @return Unauthorized access ServerError
     * */
    static ServerError create_unauthorized_error();

    /*!
     * @brief Create ServerError for invalid payload.
     *
     * @return Invalid payload ServerError
     * */
    static ServerError create_invalid_payload_error();

    /*!
     * @brief Create ServerError for problems with connection to agent
     *
     * @return Agent Unreachable ServerError
     * */
    static ServerError create_agent_unreachable_error();

    /*!
     * @brief Create ServerError for conflict.
     *
     * @return Conflict ServerError
     * */
    static ServerError create_conflict_error();

    /*!
     * @brief Create ServerError for method not implemented on agent.
     *
     * @return Not Implemented ServerError
     * */
    static ServerError create_agent_methond_not_implemented_error();

private:
    ErrorFactory() = delete;
    ErrorFactory(const ErrorFactory&) = delete;
    ErrorFactory& operator=(const ErrorFactory&) = delete;
};

}
}
}

#endif  /* PSME_ERROR_ERROR_FACTORY_HPP */
