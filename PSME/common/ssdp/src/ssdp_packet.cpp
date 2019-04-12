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
 * */

#include "ssdp/ssdp_packet.hpp"
#include "ssdp/ssdp_packet_parser.hpp"

#include <array>
#include <stdexcept>
#include <sstream>
#include <cctype>

using namespace ssdp;

constexpr const char SsdpPacket::AL[];
constexpr const char SsdpPacket::HOST[];
constexpr const char SsdpPacket::LOCATION[];
constexpr const char SsdpPacket::CACHE_CONTROL[];
constexpr const char SsdpPacket::SERVER[];
constexpr const char SsdpPacket::NT[];
constexpr const char SsdpPacket::SSDP_ALIVE_STR[];
constexpr const char SsdpPacket::SSDP_BYEBYE_STR[];
constexpr const char SsdpPacket::NTS[];
constexpr const char SsdpPacket::USN[];
constexpr const char SsdpPacket::ST[];
constexpr const char SsdpPacket::MAN[];
constexpr const char SsdpPacket::SSDP_DISCOVER[];
constexpr const char SsdpPacket::MX[];
constexpr const char SsdpPacket::EXT[];

SsdpPacket::SsdpPacket(SsdpPacket::Type type) : m_type(type) {}

SsdpPacket::Type SsdpPacket::get_type() const {
    return m_type;
}

void SsdpPacket::set_header(const std::string& name, const std::string& value) {
    m_headers.set_header(name, value);
}

const std::string& SsdpPacket::get_header(const std::string& name) const {
    return m_headers.get_header(name);
}

bool SsdpPacket::has_header(const std::string& name) const {
    return m_headers.contains(name);
}

bool SsdpPacket::is_notify() const {
    return SsdpPacket::Type::NOTIFY == m_type;
}

bool SsdpPacket::is_search_request() const {
    return SsdpPacket::Type::SEARCH_REQUEST == m_type;
}

bool SsdpPacket::is_search_response() const {
    return SsdpPacket::Type::SEARCH_RESPONSE == m_type;
}

namespace {

std::ostream& get_request_line(std::ostream& out, const SsdpPacket::Type& method) {
    out << method.to_request_line_string();
    if (SsdpPacket::Type::SEARCH_REQUEST == method
        || SsdpPacket::Type::NOTIFY == method) {
        out << " * " << ssdp::HTTP_VERSION;
    }
    else if (SsdpPacket::Type::SEARCH_RESPONSE == method) {
        out << " 200 OK";
    }
    out << ssdp::CRLF;
    return out;
}

}

std::string SsdpPacket::to_string() const {
    std::ostringstream out;
    get_request_line(out, m_type);

    for (const auto& header_value : m_headers) {
        out << header_value.first << ":" << header_value.second
            << ssdp::CRLF;
    }
    out << ssdp::CRLF;
    return out.str();
}

bool SsdpPacket::Headers::contains(const std::string& key) const {
    return find(key) != m_headers.cend();
}

void SsdpPacket::Headers::set_header(const std::string& name, const std::string& value) {
    auto it = find(name);
    if (it != m_headers.end()) {
        it->second = value;
    }
    else {
        m_headers.emplace_back(name, value);
    }
}

const std::string& SsdpPacket::Headers::get_header(const std::string& name) const {
    const auto it = find(name);
    if (it != m_headers.cend()) {
        return it->second;
    }
    throw std::runtime_error(name + " not found");
}

namespace {
    int icompare(const std::string& str1, const std::string& str2) {
        auto it1(str1.cbegin());
        auto end1(str1.cend());
        auto it2(str2.cbegin());
        auto end2(str2.cend());
        while (it1 != end1 && it2 != end2) {
            auto c1  = std::tolower(*it1);
            auto c2 = std::tolower(*it2);
            if (c1 < c2) {
                return -1;
            }
            else if (c1 > c2) {
                return 1;
            }
            ++it1;
            ++it2;
        }

        if (it1 == end1) {
            return it2 == end2 ? 0 : -1;
        }
        else {
            return 1;
        }
    }
}

SsdpPacket::Headers::Container::const_iterator SsdpPacket::Headers::find(const std::string& key) const {
    auto it = m_headers.cbegin();
    auto it_end = m_headers.cend();
    for (; it != it_end; ++it) {
        if (0 == icompare(it->first, key)) {
            return it;
        }
    }
    return it_end;
}

SsdpPacket::Headers::Container::iterator SsdpPacket::Headers::find(const std::string& key) {
    auto it = m_headers.begin();
    auto it_end = m_headers.end();
    for (; it != it_end; ++it) {
        if (0 == icompare(it->first, key)) {
            return it;
        }
    }
    return it_end;
}

namespace {

struct SsdpPacketTypeStrings {
    SsdpPacket::Type::TypeEnum m_enum;
    std::string m_request_line_string;
    std::string m_display_string;
};

const std::array<const SsdpPacketTypeStrings, 4> ssdp_packet_type_strings_array = {{
    {SsdpPacket::Type::NOTIFY, "NOTIFY", "NOTIFY"},
    {SsdpPacket::Type::SEARCH_REQUEST, "M-SEARCH", "SEARCH_REQUEST"},
    {SsdpPacket::Type::SEARCH_RESPONSE, "HTTP/1.1", "SEARCH_RESPONSE"},
    {SsdpPacket::Type::UNKNOWN, "UNKNOWN", "UNKNOWN"}
}};

}

SsdpPacket::Type SsdpPacket::Type::from_request_line_string(const std::string& str) {
    for (const auto& elem: ssdp_packet_type_strings_array) {
        if (str == elem.m_request_line_string) {
            return elem.m_enum;
        }
    }
    return Type(TypeEnum::UNKNOWN);
}

const std::string& SsdpPacket::Type::to_request_line_string() const {
    return ssdp_packet_type_strings_array.at(m_value).m_request_line_string;
}

const std::string& SsdpPacket::Type::to_string() const {
    return ssdp_packet_type_strings_array.at(m_value).m_display_string;
}
