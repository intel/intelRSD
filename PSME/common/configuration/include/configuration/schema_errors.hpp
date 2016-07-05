/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file schema_errors.hpp
 *
 * @brief The SchemaErrors collection of validation errors
 * */

#ifndef CONFIGURATION_SCHEME_ERRORS_HPP
#define CONFIGURATION_SCHEME_ERRORS_HPP

#include <string>
#include <vector>

namespace configuration {

/*! Class store collection of errors */
class SchemaErrors {
public:
    /*! SchemaErrors default constructor */
    explicit SchemaErrors() = default;

    class Error {
        /*! error list type */
        using error_list_t = std::vector<std::string>;
    public:
        /*!
         * @brief Construct error object for given property
         *
         * @param property Property name
         * @param errors_list Errors list
         */
        Error(const std::string& property,
              const error_list_t& errors_list = {});

        /*!
         * @brief Add error message to errors list
         * @param error Error string message
         */
        void add_error_message(const std::string& error_message);

        /*!
         * @brief Errors count for specific property
         * @return Errors count
         */
        std::size_t count() const;

        /*!
         * @brief Set JSON error value
         * @param value JSON error value
         */
        void set_value(const std::string& value);

        /*!
         * @brief Get JSON error value
         * @return JSON error value
         */
        const std::string& get_value() const;

        /*!
         * @brief Set path to invalid JSON property
         * @param path JSON path
         */
        void set_path(const std::string& path);

        /*!
         * @brief Get path to invalid JSON property
         * @return JSON path
         */
        const std::string& get_path() const;

        /*!
         * @brief Get all of messages from error
         * @return std::vector of std::string with error messages
         * */
        const error_list_t& get_messages() const;

        /*!
         * @brief Convert error object to string
         * @return error as string
         */
        std::string to_string() const;

    private:
        /*! JSON path to invalid property */
        std::string m_path{};
        /*! JSON property name */
        std::string m_property;
        /*! JSON invalid value */
        std::string m_value{};

        /*! Errors list */
        error_list_t m_messages;
    };

    /*!
     * @brief Add error to errors list
     * @param error Error object
     */
    void add_error(const Error& error);

    /*!
     * @brief Error count
     * @return error count
     */
    std::size_t count() const;

    /*!
     * @brief Check if there is some errors
     * @return True on success, otherwise false
     */
    bool is_valid() const;

    /*!
     * @brief Convert errors to string
     * @return errors as string
     */
    std::string to_string() const;

    /*!
     * @brief Get vector of Error
     * @return std::vector of Error
     * */
    const std::vector<Error>& get_errors() const;

private:
    using errors_t = std::vector<Error>;
    errors_t m_errors{};
};

}
#endif /* CONFIGURATION_SCHEME_ERRORS_HPP */
