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
 * @file server_error.hpp
 *
 * @brief Declaration of ServerError and ErrorDetail classes
 * */
#ifndef PSME_REST_ERROR_SERVERERROR_HPP
#define	PSME_REST_ERROR_SERVERERROR_HPP

#include "json/json.hpp"
#include <string>
#include <exception>

namespace psme {
namespace rest {
namespace error {

/*! Forward declaration */
class ErrorDetail;

/*!
 * @brief Rest ServerError for extended error handling compatible with redfish spec.
 *
 * */
class ServerError final {
public:
    /*!
     * @brief Default constructor.
     *
     * Create ServerError with INTERNAL_SERVER_ERROR HTTP status code.
     * */
    ServerError();

    /*!
     * @brief Create ServerError object with given HTTP status code.
     *
     * @param[in] http_status_code HTTP status code associated with the error.
     * */
    explicit ServerError(std::uint32_t http_status_code);

    /*! @brief Default copy constructor */
    ServerError(const ServerError&) = default;

    /*! @brief Assignment operator */
    ServerError& operator=(const ServerError&) = default;

    /*! @brief Destructor */
    ~ServerError();

    /*!
     * @brief Set HTTP status code.
     *
     * @param[in] http_status_code HTTP status code associated with the error.
     * */
    void set_http_status_code(std::uint32_t http_status_code);

    /*!
     * @brief Get HTTP status code.
     *
     * @return HTTP status code associated with the error.
     * */
    std::uint32_t get_http_status_code() const { return m_http_status_code; }

    /*!
     * @brief Set error code.
     *
     * @param[in] code  String indicating a specific error or message
     * (not to be confused with the HTTP status code). This code can be used
     * to access a detailed message from message registry.
     * */
    void set_code(const std::string& code);

    /*!
     * @brief Error code getter.
     *
     * @return Error code.
     * */
    const std::string& get_code() const;

    /*!
     * @brief Set error message.
     *
     * @param[in] message  A human readable error message indicating the semantics
     * associated with the error.
     * */
    void set_message(const std::string& message);

    /*!
     * @brief Error message getter.
     *
     * @return Error message.
     * */
    const std::string& get_message() const;

    /*!
     * @brief Add extended message.
     *
     * @param[in] info  Additional human readable error message.
     * */
    void add_extended_message(const std::string& info);

    /*!
     * @brief Convert error JSON value to string
     *
     * @return JSON value as a string
     * */
    std::string as_string() const;

private:
    std::uint32_t m_http_status_code;
    json::Value m_error;
};

}
}
}

#endif	/* PSME_REST_ERROR_SERVERERROR_HPP */

