/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file schema_errors.hpp
 *
 * @brief The SchemaErrors collection of validation errors
 * */

#pragma once

#include <string>
#include <vector>
#include <set>

namespace configuration {

/*! Class store collection of errors */
class SchemaErrors {
public:
    class Error {
    private:
        /*! Error list type */
        using Paths = std::set<std::string>;
        /*! JSON paths to invalid properties */
        Paths m_paths{};
        /*! Error message */
        std::string m_message{};

    public:

        /*!
         * @brief Construct error object for given property path.
         * @param[in] message Error message.
         * @param[in] path JSON property path.
         * */
        Error(const std::string& message, const std::string& path);

        /*!
         * @brief Add another property path related with this error message.
         * @param[in] path JSON property path.
         * */
        void add_path(const std::string& path) {
            m_paths.insert(path);
        }

        /*!
         * @brief Add collection of paths to properties related with error.
         * @param[in] paths JSON property paths collection
         * */
        void add_paths(const Paths& paths) {
            m_paths.insert(paths.cbegin(), paths.cend());
        }

        /*!
         * @brief Gets paths to invalid JSON properties.
         * @return JSON paths.
         * */
        const Paths& get_paths() const {
            return m_paths;
        }

        /*!
         * @brief Gets message for invalid properties.
         * @return Error message.
         * */
        const std::string& get_message() const {
            return m_message;
        }

        /*!
         * @brief Convert error object to string.
         * @return Error as string.
         */
        std::string to_string() const;
    };

    /* Alias */
    using Errors = std::vector<Error>;

    /*! @brief SchemaErrors default constructor. */
    explicit SchemaErrors() = default;

    /*!
     * @brief Add error to errors list.
     * @param[in] error Error object.
     * */
    void add_error(const Error& error);

    /*!
     * @brief Gets count of errors
     * @return Error count
     * */
    std::size_t count() const;

    /*!
     * @brief Check if there is some errors
     * @return True on success, otherwise false
     * */
    bool is_valid() const;

    /*!
     * @brief Convert errors to string
     * @return Errors as string
     * */
    std::string to_string() const;

    /*!
     * @brief Get vector of Error
     * @return Returns std::vector of Error
     * */
    const Errors& get_errors() const;

private:
    Errors m_errors{};
};

}
