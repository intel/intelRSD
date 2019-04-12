/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file server_exception.hpp
 *
 * @brief Declaration of ServerException class
 * */
#pragma once



#include "server_error.hpp"
#include <exception>



namespace psme {
namespace rest {
namespace error {

/*!
 * @brief Rest ServerException.
 *
 * Generic Rest ServerException containing ServerError describing error details.
 * */
class ServerException : public std::exception {
private:
    const ServerError m_error;
    mutable std::string m_what{};


    /* Prepares m_what field returned on what() method call */
    void make_what() const;


public:
    /*!
     * @brief Constructor
     *
     * Takes a ServerError object describing the error.
     *
     * @param[in] error ServerError object describing error details
     * */
    explicit ServerException(const ServerError& error);


    /*! @brief Default copy constructor. */
    ServerException(const ServerException&) = default;


    /*! @brief Assignment operator */
    ServerException& operator=(const ServerException&) = default;


    /*! @brief Destructor */
    virtual ~ServerException() noexcept;


    /*!
     * @brief Get ServerError object associated with the exception.
     *
     * @return ServerError object describing error details.
     * */
    const ServerError& get_error() const {
        return m_error;
    }


    /*!
     * @brief Returns a C-style character string describing the general cause of
     * the current error.
     *
     * @return Description of the current error
     * */
    virtual const char* what() const noexcept;
};

}
}
}


