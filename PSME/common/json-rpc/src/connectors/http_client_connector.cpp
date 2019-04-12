/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file connectors/http_client_connector.cpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    httpclient.cpp
 * @date    02.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "json-rpc/connectors/http_client_connector.hpp"

#include "json-rpc/common.hpp"



using namespace json_rpc;
using namespace json_rpc::common;

namespace {

class CurlInitializer {
public:
    CurlInitializer() {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    CurlInitializer(const CurlInitializer&) = delete;
    CurlInitializer(CurlInitializer&&) = delete;

    CurlInitializer& operator=(const CurlInitializer&) = delete;
    CurlInitializer& operator=(CurlInitializer&&) = delete;

    ~CurlInitializer() {
        curl_global_cleanup();
    }
};

// See here: http://curl.haxx.se/libcurl/c/curl_global_init.html
static CurlInitializer curl_init{};

// Taken from http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
static size_t curl_write_function(void* ptr, size_t size, size_t nmemb, std::string *s) {
    size_t chunk_length = size * nmemb;
    size_t old_length = s->size();
    try {
        s->resize(old_length + chunk_length);
    }
    catch (const std::bad_alloc&) {
        // TODO - handle it properly
        return 0;
    }

    char* source = static_cast<char*>(ptr);
    std::copy(source, source + chunk_length, s->begin() + old_length);
    return chunk_length;
}

}

HttpClientConnector::HttpClientConnector(const std::string& url, long timeout_ms):
        m_url(url), m_timeout_ms(timeout_ms), m_curl{curl_easy_init(), &curl_easy_cleanup} {
    if (nullptr == m_curl) {
        throw JsonRpcException(ERROR_CLIENT_CONNECTOR, "Unable to initialize HTTP client connector");
    }
}

HttpClientConnector::~HttpClientConnector() {
}

std::string HttpClientConnector::send_request(const std::string& message) {

    if (nullptr == m_curl) {
        throw JsonRpcException(ERROR_CLIENT_CONNECTOR, "HTTP client connector not initialized");
    }

    curl_easy_setopt(m_curl.get(), CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_curl.get(), CURLOPT_TIMEOUT_MS, m_timeout_ms);
    curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, ::curl_write_function);
    curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDS, message.c_str());

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);

    std::string data{};
    curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, &data);

    CURLcode result = curl_easy_perform(m_curl.get());
    curl_slist_free_all(headers);

    if (result != CURLE_OK)
    {
        if (CURLE_COULDNT_CONNECT == result) {
            throw JsonRpcException(ERROR_CLIENT_CONNECTOR, "Curl error: could not connect to: " + m_url);
        }
        else if (CURLE_OPERATION_TIMEDOUT == result) {
            throw JsonRpcException(ERROR_CLIENT_CONNECTOR, "Curl error: operation timed out");
        }
        throw JsonRpcException(ERROR_CLIENT_CONNECTOR, std::string{"Curl error: code "} + std::to_string(int(result)));
    }

    long http_code = 0;
    curl_easy_getinfo(m_curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

    constexpr long HTTP_OK_STATUS = 200;
    if (http_code != HTTP_OK_STATUS) {
        throw JsonRpcException(ERROR_CLIENT_INVALID_RESPONSE,
            "Curl error: unexpected http code: " + std::to_string(http_code));
    }
    return data;
}
