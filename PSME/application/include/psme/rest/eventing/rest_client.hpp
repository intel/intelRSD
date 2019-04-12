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
 * @file rest_client.hpp
 * @brief REST client using curl
 * */

#pragma once
#include "psme/rest/server/methods.hpp"

#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <memory>

namespace psme {
namespace rest {
namespace eventing {

using namespace psme::rest::server;

/*!
 * @brief RestClient class declaration
 */
class RestClient {
public:
    /*!
     * @brief Response class
     */
    class Response {
        int m_response_code{};
        std::map<std::string, std::string> m_headers{};
        std::string m_body{};
    public:
        /*!
         * @brief Get response code
         *
         * @return Response code
         */
        int get_response_code() const {
            return m_response_code;
        }

        /*!
         * @brief Set response code
         *
         * @param response_code Response code
         */
        void set_response_code(int response_code) {
            m_response_code = response_code;
        }

        /*!
         * @brief Get header
         *
         * @param key Key
         *
         * @return Header
         */
        const std::string& get_header(const std::string& key) {
            return m_headers[key];
        }

        /*!
         * @brief Set header
         *
         * @param key Key
         * @param value Key value
         */
        void set_header(const std::string& key, const std::string& value) {
            m_headers[key] = value;
        }

        /*!
         * @brief Clear headers
         */
        void clear_headers() {
            m_headers.clear();
        }

        /*!
         * @brief Get body
         *
         * @return Body
         */
        const std::string& get_body() const {
            return m_body;
        }

        /*!
         * @brief Set body
         *
         * @param body Body
         */
        void set_body(const std::string& body) {
            m_body = body;
        }

        /*!
         * @brief Append body
         *
         * @param data Content data
         * @param len Data size
         */
        void append_body(const char *data, size_t len) {
            m_body.append(data, len);
        }
    };

    /*!
     * @brief Upload class
     */
    class Upload {
        std::string m_data;
        size_t m_pos;
    public:

        /*!
         * @brief Constructor
         *
         * @param data Data
         */
        Upload(const std::string& data) : m_data(data), m_pos(0) {
        }

        /*!
         * @brief Get remining data size
         *
         * @return Remaining data size
         */
        size_t remaining() const {
            return m_data.length() - m_pos;
        }

        /*!
         * @brief Get copy out size
         *
         * @return Copy out size
         */
        size_t copy_out(void *dst, size_t size);
    };

    /*!
     * @brief Constructor
     *
     * @param base_url Base URL
     */
    RestClient(const std::string& base_url) : m_base_url(base_url) {
    }

    /*!
     * @brief Set basic auth
     *
     * @param user User name
     * @param password Password
     */
    void set_basic_auth(const std::string& user, const std::string& password);

    /*!
     * @brief Clear basic auth
     */
    void clear_basic_auth();

    /*!
     * @brief Test if basic auth flag is set
     *
     * @return Basic auth flag
     */
    bool has_basic_auth();

    /*!
     * @brief Set default accept
     *
     * @param accept Default accept flag
     */
    void set_default_accept(const std::string& accept);

    /*!
     * @brief Set default content type
     *
     * @param content_type Content type
     */
    void set_default_content_type(const std::string& content_type);

    /*!
     * @brief Set timeout
     *
     * @param timeout Timeout
     */
    void set_timeout(long timeout);

    /*!
     * @brief Get timeou
     *
     * @return Timeout
     */
    long get_timeout() const;

    /*!
     * @brief GET method
     *
     * @param url URL
     *
     * @return Response
     */
    Response get(const std::string& url);

    /*!
     * @brief POST method
     *
     * @param url URL
     * @param data Data
     */
    Response post(const std::string& url, const std::string& data);

    /*!
     * @brief PUT method
     *
     * @param url URL
     * @param data Data
     */
    Response put(const std::string& url, const std::string& data);

    /*!
     * @brief DEL method
     *
     * @param url URL
     */
    Response del(const std::string& url);


private:
    Response rest_method_template(Method method, const std::string& url,
            const std::string& data);

    std::string m_base_url;
    long m_timeout{10000};
    std::string m_accept{};
    std::string m_content_type{};
    std::string m_basic_auth{};

};

}
}
}

