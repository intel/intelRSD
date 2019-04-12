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
 * @file uri.hpp
 *
 * @brief URI declaration
 **/

#pragma once

#include <string>

namespace ssdp {

/*! Simplified URI for SSDP service location header manipulation */
class URI final {
public:
    /*!
     * Constructor
     * Parses URI from string.
     * @param[in] uri String representation of URI
     */
    explicit URI(const std::string& uri);

    /*!
     * Converts URI to string representation.
     * @return String representation of URI.
     */
    std::string to_string() const;

    /*!
     * Gets scheme part of URI.
     * @return Scheme part of URI.
     */
    const std::string& get_scheme() const {
        return m_scheme;
    }

    /*!
     * Sets scheme part of URI.
     * @param scheme Scheme part of URI.
     */
    void set_scheme(const std::string& scheme);

    /*!
     * Gets user info part of URI.
     * @return user info part of URI.
     */
    const std::string& get_user_info() const {
        return m_userInfo;
    }

    /*!
     * Sets user info part of URI.
     * @param user_info User info part of URI.
     */
    void set_user_info(const std::string& user_info);

    /*!
     * Gets host part of URI.
     * @return Host part of URI.
     */
    const std::string& get_host() const {
        return m_host;
    }

    /*!
     * Sets host part of URI.
     * @param host Host part of URI.
     */
    void set_host(const std::string& host);

    /*!
     * Gets port part of URI.
     * If port was not specified, the well-known port number
     * for scheme (like 80 for http) is returned (if known).
     * Otherwise 0 is returned.
     *
     * @return Port part of URI.
     */
    unsigned short get_port() const;

    /*!
     * Sets port part of URI.
     * @param port Port part of URI.
     */
    void set_port(unsigned short port);

    /*!
     * Gets authority part of URI.
     * @return Authority part of URI.
     */
    std::string get_authority() const;

    /*!
     * Sets authority part of URI.
     * @param authority Authority part of URI.
     */
    void set_authority(const std::string& authority);

    /*!
     * Gets path and query part of URI.
     * @return Path and query part of URI.
     */
    const std::string& get_path_and_query() const {
        return m_path_and_query;
    }

    /*!
     * @return True if URI is empty (has empty components).
     */
    bool empty() const;

private:
    bool equals(const URI& uri) const;

    void parse(const std::string& uri);

    void parse_authority(std::string::const_iterator& it, const std::string::const_iterator& end);

    void parse_host_and_port(std::string::const_iterator& it, const std::string::const_iterator& end);

    void parse_path(std::string::const_iterator& it, const std::string::const_iterator& end);

    std::string m_scheme{};
    std::string m_userInfo{};
    std::string m_host{};
    unsigned short m_port{0};
    std::string m_path_and_query{};
};

} // namespace ssdp
