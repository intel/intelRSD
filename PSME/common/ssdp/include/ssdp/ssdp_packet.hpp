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
 * @file ssdp_packet.hpp
 *
 * @brief Declaration of SsdpPacket.
 * */

#pragma once

#include <string>
#include <vector>

namespace ssdp {

/*!
 * Class for storing SSDP datagram data.
 *
 * SsdpPacket consists of packet type and headers.
 */
class SsdpPacket {
public:
    /*! Service location header */
    static constexpr const char AL[] = "AL";
    /*! Host header */
    static constexpr const char HOST[] = "Host";
    /*! Location header */
    static constexpr const char LOCATION[] = "Location";
    /*! HTTP Cache-Control header */
    static constexpr const char CACHE_CONTROL[] = "Cache-Control";
    /*! HTTP Server header */
    static constexpr const char SERVER[] = "Server";
    /*! Notification type header */
    static constexpr const char NT[] = "NT";
    /*! Notification sub type header */
    static constexpr const char NTS[] = "NTS";
    /*! Notification sub type value */
    static constexpr const char SSDP_ALIVE_STR[] = "ssdp:alive";
    /*! Notification sub type value */
    static constexpr const char SSDP_BYEBYE_STR[] = "ssdp:byebye";
    /*! Unique Service Name header */
    static constexpr const char USN[] = "USN";
    /*! Search target header */
    static constexpr const char ST[] = "ST";
    /*! Header required by HTTP Extension Framework. Shall be "ssdp:discover". */
    static constexpr const char MAN[] = "MAN";
    /*! Value of MAN header */
    static constexpr const char SSDP_DISCOVER[] = "\"ssdp:discover\"";
    /*! Header containing maximum wait time in seconds. */
    static constexpr const char MX[] = "MX";
    /*! Required for backward compatibility with UPnP 1.0. */
    static constexpr const char EXT[] = "Ext";

    /*! Represents SSDP packet type. */
    class Type {
    public:
        /*!
         * Enumeration of available SSDP packet types.
         */
        enum TypeEnum : std::uint8_t {
            NOTIFY,
            SEARCH_REQUEST,
            SEARCH_RESPONSE,
            UNKNOWN
        };

        /*!
         * Converts string to corresponding Type.
         * @param type_string text to be converted to Type.
         * @return corresponding Type or UNKNOWN if not recognized.
         * */
        static Type from_request_line_string(const std::string& type_string);

        /*!
         * Constructor.
         * @param value TypeEnum value.
         * */
        constexpr Type(TypeEnum value) : m_value(value) { }

        /*!
         * Converts Type to const char* (cstring).
         * @return String representation of Type
         *         compatible with SSDP request line format.
         * */
        const std::string& to_request_line_string() const;

        /*!
         * Converts Type to const char* (cstring).
         * @return String representation of Type.
         * */
        const std::string& to_string() const;

        /*!
         * Conversion operator
         *
         * @return TypeEnum value of this Type.
         * */
        constexpr operator TypeEnum() const {
            return static_cast<TypeEnum>(m_value);
        }

    private:
        std::uint8_t m_value{};
    };

    /*!
     * Constructor. Creates SsdpPacket of given type.
     * @param type SSDP packet type.
     */
    explicit SsdpPacket(Type type);

    /*!
     * SSDP packet type getter.
     * @return type of SSDP packet.
     */
    Type get_type() const;

    /*!
     * Tests if it is SSDP search request packet.
     * @return true if packet is a SSDP search request
     */
    bool is_search_request() const;

    /*!
     * Tests if it is SSDP notify packet.
     * @return true if packet is a SSDP notification
     */
    bool is_notify() const;

    /*!
     * Tests if it is SSDP search response packet.
     * @return true if packet is a SSDP search response
     */
    bool is_search_response() const;

    /*!
     * SSDP header setter.
     * @param name name of header to be set.
     * @param value value of header to be set.
     */
    void set_header(const std::string& name, const std::string& value);

    /*!
     * SSDP header getter.
     * @param name name of requested header.
     * @return Value of requested header.
     */
    const std::string& get_header(const std::string& name) const;

    /*!
     * Tests for header presence.
     * @param name name of requested header.
     * @return true if requested header is present, false otherwise.
     */
    bool has_header(const std::string& name) const;

    /*!
     * Converts SsdpPacket to string representation.
     * String representation is ready to be send out via Socket.
     * @return String representation of SsdpPacket.
     */
    std::string to_string() const;

private:

    class Headers {
        using Container = std::vector<std::pair<std::string, std::string>>;
        Container m_headers{};
    public:
        Container::const_iterator find(const std::string& key) const;
        Container::iterator find(const std::string& key);
        void set_header(const std::string& name, const std::string& value);
        const std::string& get_header(const std::string& name) const;
        bool contains(const std::string& name) const;

        Container::const_iterator begin() const { return m_headers.cbegin(); }
        Container::const_iterator end() const { return m_headers.cend(); }
        Container::iterator begin() { return m_headers.begin(); }
        Container::iterator end() { return m_headers.end(); }
        Container::size_type size() { return m_headers.size(); }
    };

    Type m_type;
    Headers m_headers{};
};

/*!
 * Used as a key in associative containers.
 */
class SsdpPacketKey {
public:
    /*! Constructor */
    SsdpPacketKey() {}

    /*!
     * Constructor
     * param[in] type SsdpPacket type.
     * param[in] sub_type SsdpPacket notification sub type.
     */
    SsdpPacketKey(const SsdpPacket::Type::TypeEnum& type, const std::string& sub_type = {})
        : m_type(type), m_sub_type(sub_type) { }

    /*!
     * Constructor
     * param[in] packet SsdpPacket
     */
    explicit SsdpPacketKey(const SsdpPacket& packet) : m_type{packet.get_type()} {
        if (packet.has_header(SsdpPacket::NTS)) {
            m_sub_type = packet.get_header(SsdpPacket::NTS);
        }
    }

    SsdpPacketKey(const SsdpPacketKey& packet_key) = default;
    SsdpPacketKey& operator=(const SsdpPacketKey& packet_key) = default;

    /*! Equal operator */
    bool operator==(const SsdpPacketKey& other) const {
        return m_type == other.m_type
               && m_sub_type == other.m_sub_type;
    }

    /*! Less operator */
    bool operator<(const SsdpPacketKey& other) const {
        if (m_type == other.m_type) {
            return m_sub_type < other.m_sub_type;
        }
        return m_type < other.m_type;
    }
private:
    friend std::hash<ssdp::SsdpPacketKey>;
    SsdpPacket::Type::TypeEnum m_type{SsdpPacket::Type::UNKNOWN};
    std::string m_sub_type{};
};

}

namespace std {

// Allows SsdpPacketKey to be used as unordered_map key
template <>
struct hash<ssdp::SsdpPacketKey> {
    std::size_t operator()(const ssdp::SsdpPacketKey& type) const {
        if (!type.m_sub_type.empty()) {
            return type.m_type ^ (std::hash<std::string>()(type.m_sub_type) << 2);
        }
        else {
            return type.m_type;
        }
    }
};

}
