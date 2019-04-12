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
 * @file ssdp_packet_parser.hpp
 *
 * @brief Declaration of SsdpPacketParser.
 * */

#pragma once

#include "ssdp_packet.hpp"

#include <stdexcept>
#include <string>

namespace ssdp {

/*!
 * @brief Exception thrown when error condition occurs
 * while parsing SSDP packet.
 */
class PacketParserException : public std::runtime_error {
public:
    /*!
     * @brief Constructor
     * @param msg Error description
     */
    explicit PacketParserException(const std::string& msg);

    /*!
     * @brief Constructor
     */
    PacketParserException(const PacketParserException&) = default;

    /*! @brief Destructor */
    virtual ~PacketParserException();
};

/*! HTTP version string */
extern const char HTTP_VERSION[];
/*! Carriage return/line feed constant */
extern const char CRLF[];

std::string& trim_right(std::string& str);

/*!
 * @brief SSDP packet parser.
 *
 * Parses SSDP datagram packets and creates SsdpPacket objects.
 */
template<typename Container>
class SsdpPacketParser final {
public:
    using Size = typename Container::size_type;

    /*! Constructor
     * @param container SSDP datagram payload container.
     * @param length SSDP datagram payload length.
     */
    SsdpPacketParser(const Container& container, Size length)
            : m_begin(container.cbegin()),
              m_current(m_begin),
              m_end(m_begin + length) {
        if (length > container.size()) {
            throw PacketParserException("Length out of range");
        }
    }

    /*!
     * Parses SSDP datagram payload into SsdpPacket object.
     * @return SsdpPacket object.
     */
    SsdpPacket parse();

private:
    std::string next_token();
    void skip_spaces();
    SsdpPacket parse_request_line();
    bool parse_headers(SsdpPacket& packet);

    using Iterator = typename Container::const_iterator;
    Iterator m_begin;
    Iterator m_current;
    Iterator m_end;
};

template<typename Iterator>
SsdpPacket SsdpPacketParser<Iterator>::parse() {
    auto ssdp_packet = parse_request_line();
    parse_headers(ssdp_packet);
    return ssdp_packet;
}

template<typename Iterator>
SsdpPacket SsdpPacketParser<Iterator>::parse_request_line() {
    skip_spaces();
    // "NOTIFY", "M-SEARCH", "HTTP/1.1"
    auto method_or_resp = next_token();
    skip_spaces();
    // * or 200
    next_token();
    skip_spaces();
    // HTTP/1.1 or OK
    next_token();
    skip_spaces();

    return SsdpPacket(SsdpPacket::Type::from_request_line_string(method_or_resp));
}

template<typename Iterator>
bool SsdpPacketParser<Iterator>::parse_headers(SsdpPacket& packet) {
    std::string name{};
    std::string value{};

    while (m_current != m_end && *m_current != '\r' && *m_current != '\n') {
        name.clear();
        value.clear();
        for (; m_current != m_end && *m_current != ':' && *m_current != '\n'; ++m_current) {
            name += *m_current;
        }
        // ignore invalid header lines
        if ('\n' == *m_current) {
            ++m_current;
            continue;
        }
        if (*m_current != ':') {
            throw PacketParserException("No colon found");
        }
        if (m_current != m_end) {
            ++m_current; // ':'
        }
        while (m_current != m_end && std::isspace(*m_current) && *m_current != '\r' && *m_current != '\n') {
            ++m_current;
        }
        for (; m_current != m_end && *m_current != '\r' && *m_current != '\n'; ++m_current) {
            value += *m_current;
        }
        if (*m_current == '\r') {
            ++m_current;
        }
        if (*m_current == '\n') {
            ++m_current;
        }
        else if (m_current != m_end) {
            throw PacketParserException("No CRLF found");
        }
        // folding
        while (*m_current == ' ' || *m_current == '\t') {
            while (m_current != m_end && *m_current != '\r' && *m_current != '\n') {
                value += *m_current;
                ++m_current;
            }
            if (*m_current == '\r') {
                ++m_current;
            }
            if (*m_current == '\n') {
                ++m_current;
            }
            else if (m_current != m_end) {
                throw PacketParserException("No CRLF found");
            }
        }
        packet.set_header(name, trim_right(value));
    }

    while (m_current != m_end && *m_current != '\n') {
        ++m_current;
    }

    return true;
}

template<typename Iterator>
std::string SsdpPacketParser<Iterator>::next_token() {
    std::string token{};
    for (; m_current != m_end && !std::isspace(*m_current); ++m_current) {
        token += *m_current;
    }
    return token;
}

template<typename Iterator>
void SsdpPacketParser<Iterator>::skip_spaces() {
    for (; m_current != m_end && std::isspace(*m_current); ++m_current)
        ;
}

}
