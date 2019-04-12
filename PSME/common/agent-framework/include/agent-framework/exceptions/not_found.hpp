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
 * @file not_found.hpp
 *
 * @brief Declaration of not found exception
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Not found exception class */
class NotFound final : public GamiException {
public:

    /*! @brief Exception JSON field */
    static const constexpr char URI[] = "uri";


    explicit NotFound(const std::string& msg);


    explicit NotFound(const std::string& msg, const std::string& uri);


    NotFound(const NotFound&) = default;


    NotFound& operator=(const NotFound&) = default;


    NotFound(NotFound&&) = default;


    NotFound& operator=(NotFound&&) = default;


    /*! @brief Destructor */
    ~NotFound();

    /*!
     * @brief Get field value from JSON data.
     * @param data Exception's JSON data.
     * @param should_return_empty Should method return empty string or "not specified" text.
     * @return Field value as string or empty string if data does not contain field name.
     * */
    static std::string get_uri_from_json_data(const json::Json& data, bool should_return_empty = true);

    /*!
     * @brief Creates JSON data object from invalid URI.
     * @param uri Requested invalid URI
     * @return JSON data object with URI
     */
    json::Json create_json_data_from_uri(const std::string& uri);
};

} // namespace exceptions
} // namespace agent_framework
