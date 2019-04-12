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
 * @file command_message_header.hpp
 *
 * @brief LLDP command message header
 * */

#pragma once

#include "message_header.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP command header class
 * */
class CommandMessageHeader final : public MessageHeader {
public:
    /*!
     * @enum CmdCode
     * @brief LLDP command code
     *
     * @var CmdCode::GET_STATS
     * LLDP get statistic command code
     *
     * @var CmdCode::GET_TLV
     * LLDP get TLV info command code
     *
     * @var CmdCode::SET_TLV
     * LLDP set TLV info command code
     *
     * @var CmdCode::GET_LLDP
     * LLDP get configuration command code
     *
     * @var CmdCode::SET_LLDP
     * LLDP set configuration command code
     *
     * @var CmdCode::UNKNOWN
     * LLDP unknown command code
     *
     * */
    enum class CmdCode : uint16_t {
        GET_STATS,
        GET_TLV,
        SET_TLV,
        GET_LLDP,
        SET_LLDP,
        UNKNOWN = 0xFF
    };

    /*!
     * @enum CmdType
     * @brief LLDP command type.
     *
     * @var CmdType::UNKNOWN
     * LLDP unknown command type
     *
     * @var CmdType::RESPONSE
     * LLDP response command type
     *
     * @var CmdType::REQUEST
     * LLDP request command type
     *
     * @var CmdType::MODIFY
     * LLDP modify command type
     *
     * */
    enum class CmdType : char {
        UNKNOWN = '.',
        RESPONSE = 'R',
        REQUEST = 'C',
        MODIFY = 'M',
    };

    /*!
     * @enum CmdOption
     * @brief LLDP command option.
     *
     * @var CmdOption::LOCAL
     * LLDP local command option
     *
     * @var CmdOption::NEIGHBOR
     * LLDP neighbor command option
     *
     * @var CmdOption::ARG
     * LLDP argument command option
     *
     * @var CmdOption::ARGVAL
     * LLDP argument-value command option
     *
     * @var CmdOption::CONFIG
     * LLDP get configuration command option
     *
     * */
    enum class CmdOption : uint32_t {
        LOCAL     = 0x0001,
        NEIGHBOR  = 0x0002,
        ARG       = 0x0004,
        ARGVAL    = 0x0008,
        CONFIG    = 0x0010
    };

    /*! Default constructor */
    explicit CommandMessageHeader();

    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     * @param[in] type Command type
     * @param[in] code Command code
     * @param[in] option Command option
     *
     * */
    explicit CommandMessageHeader(const std::string& ifname,
                    CmdType type, CmdCode code, std::uint32_t option);

    /*!
     * @brief Parse raw message
     *
     * @param[in] raw Raw message
     *
     * */
    void parse_raw(const std::string& raw) override;

    /*!
     * @brief Get raw message
     *
     * @return Raw message
     *
     * */
    std::string get_raw() const override;

    /*!
     * @brief Get raw message size
     *
     * @return Raw message size
     *
     * */
    size_t get_raw_size() const override;

    /*!
     * @brief Get command type
     *
     * @return Command type
     *
     * */
    CmdType get_type() const { return m_type; }

    /*!
     * @brief Get message version
     *
     * @return Message version
     *
     * */
    std::uint16_t get_version() const { return m_version; }

    /*!
     * @brief Get command code
     *
     * @return Command code
     *
     * */
    CmdCode get_code() const { return m_code; }

    /*!
     * @brief Get command option
     *
     * @return Command option
     *
     * */
    std::uint32_t get_option() const { return m_option; }

    /*!
     * @brief Get interface name
     *
     * @return Interface name
     *
     * */
   const std::string& get_ifname() const { return m_ifname; }

    static constexpr int OFFSET_TYPE{0};
    static constexpr int OFFSET_VERSION{1};
    static constexpr int OFFSET_CODE{2};
    static constexpr int OFFSET_OPTIONS{4};
    static constexpr int OFFSET_IFNAME_LEN{12};
    static constexpr int OFFSET_IFNAME{14};

private:
    CmdType m_type{CmdType::UNKNOWN};
    CmdCode m_code{CmdCode::UNKNOWN};
    std::uint32_t m_option{0};
    std::uint16_t m_version{};
    std::string m_ifname{};
    static constexpr int VERSION{3};
};

}
}
}
}
