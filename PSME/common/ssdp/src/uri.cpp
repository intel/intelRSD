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
 **/

#include "ssdp/uri.hpp"
#include <stdexcept>
extern "C" {
#include <netdb.h>
}

namespace ssdp {

namespace {

unsigned short get_port_from_scheme(const std::string& scheme) {
    if (scheme == "http")
        return 80;
    else if (scheme == "https")
        return 443;
    else {
        struct servent* se = getservbyname(scheme.c_str(), nullptr);
        if (se) {
            return (ntohs)(uint16_t(se->s_port));
        }
        return 0;
    }
}

bool try_parse_unsigned(const std::string& num_str, unsigned long& num) {
    try {
        num = std::stoul(num_str);
        return true;
    } catch (...) {
        /*ignore*/
    }
    return false;
}

}

URI::URI(const std::string& uri) {
    parse(uri);
}

std::string URI::to_string() const {
    std::string uri;

    uri = m_scheme;
    uri += ':';
    std::string auth = get_authority();
    if (!auth.empty() || m_scheme == "file") {
        uri.append("//");
        uri.append(auth);
    }
    if (!m_path_and_query.empty()) {
        if (!auth.empty() && m_path_and_query[0] != '/')
            uri += '/';
        uri += m_path_and_query;
    }

    return uri;
}

void URI::set_scheme(const std::string& scheme) {
    m_scheme = scheme;

    if (m_port == 0) {
        m_port = get_port_from_scheme(scheme);
    }
}

void URI::set_user_info(const std::string& user_info) {
    m_userInfo = user_info;
}

void URI::set_host(const std::string& host) {
    m_host = host;
}

unsigned short URI::get_port() const {
    if (m_port == 0) {
        return get_port_from_scheme(m_scheme);
    }
    else {
        return m_port;
    }
}

void URI::set_port(unsigned short port) {
    m_port = port;
}

std::string URI::get_authority() const {
    std::string auth;
    if (!m_userInfo.empty()) {
        auth.append(m_userInfo);
        auth += '@';
    }
    if (m_host.find(':') != std::string::npos) {
        auth += '[';
        auth += m_host;
        auth += ']';
    }
    else {
        auth.append(m_host);
    }
    if (m_port) {
        auth += ':';
        auth.append(std::to_string(uint(m_port)));
    }
    return auth;
}

void URI::set_authority(const std::string& authority) {
    m_userInfo.clear();
    m_host.clear();
    m_port = 0;
    std::string::const_iterator beg = authority.begin();
    std::string::const_iterator end = authority.end();
    parse_authority(beg, end);
}

bool URI::empty() const {
    return m_scheme.empty() && m_host.empty() && m_path_and_query.empty();
}

void URI::parse(const std::string& uri) {
    auto it = uri.begin();
    auto end = uri.end();
    if (it == end) {
        return;
    }
    if (*it != '/' && *it != '.' && *it != '?' && *it != '#') {
        std::string scheme;
        while (it != end && *it != ':' && *it != '?' && *it != '#' && *it != '/') {
            scheme += *it++;
        }
        if (it != end && *it == ':') {
            ++it;
            if (it == end) {
                throw std::runtime_error("URI scheme must be followed by authority or path");
            }
            set_scheme(scheme);
            if (*it == '/') {
                ++it;
                if (it != end && *it == '/') {
                    ++it;
                    parse_authority(it, end);
                }
                else {
                    --it;
                }
            }
            parse_path(it, end);
        }
        else {
            it = uri.begin();
            parse_path(it, end);
        }
    }
}

void URI::parse_authority(std::string::const_iterator& it, const std::string::const_iterator& end) {
    std::string userInfo{};
    std::string part{};
    while (it != end && *it != '/' && *it != '?' && *it != '#') {
        if (*it == '@') {
            userInfo = part;
            part.clear();
        }
        else {
            part += *it;
        }
        ++it;
    }
    std::string::const_iterator pbeg = part.begin();
    std::string::const_iterator pend = part.end();
    parse_host_and_port(pbeg, pend);
    m_userInfo = userInfo;
}

void URI::parse_host_and_port(std::string::const_iterator& it, const std::string::const_iterator& end) {
    if (it == end) {
        return;
    }
    std::string host;
    if ('[' == *it) {
        // IPv6 address
        ++it;
        while (it != end && *it != ']') {
            host += *it++;
        }
        if (it == end) {
            throw std::runtime_error("Unterminated IPv6 address");
        }
        ++it;
    }
    else {
        while (it != end && *it != ':') {
            host += *it++;
        }
    }
    if (it != end && *it == ':') {
        ++it;
        std::string port;
        while (it != end) {
            port += *it++;
        }
        if (!port.empty()) {
            unsigned long nport = 0;
            if (try_parse_unsigned(port, nport) && nport > 0 && nport < 65536) {
                m_port = std::uint16_t(nport);
            } else {
                throw std::runtime_error("Invalid port number");
            }
        }
        else {
            m_port = get_port_from_scheme(m_scheme);
        }
    }
    else {
        m_port = get_port_from_scheme(m_scheme);
    }
    m_host = host;
}

void URI::parse_path(std::string::const_iterator& it, const std::string::const_iterator& end) {
    m_path_and_query = std::string(it, end);
}

} // namespace ssdp
