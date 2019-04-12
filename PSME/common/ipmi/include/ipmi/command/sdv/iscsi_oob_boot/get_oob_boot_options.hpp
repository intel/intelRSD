/*!
 * @brief Get OOB Boot Options command Base Class declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_oob_boot_option.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOB Boot Options.
 */
class GetOobBootOptions : public Request {
public:

    enum class ParameterType : uint8_t {
        CONTROL = 0X00,
        INITIATOR = 0X01,
        NIC = 0X02,
        TARGET = 0X03,
        HEAP = 0X04,
        RESET = 0x7f
    };


    /*!
     * @brief Default constructor.
     */
    GetOobBootOptions(ParameterType id, uint8_t index = 0);


    /*! Copy constructor */
    GetOobBootOptions(const GetOobBootOptions&) = default;


    /*! Assignment operator */
    GetOobBootOptions& operator=(const GetOobBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const = 0;


    uint8_t get_parameter_id() const { return m_id; }


    uint8_t get_parameter_index() const { return m_index; }


protected:

    void push_uint16(std::vector<uint8_t>& output, uint16_t input) const;


private:

    uint8_t m_id{};
    uint8_t m_index{};

};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOB Boot option.
 */
class GetOobBootOptions : public Response {
public:

    enum class IPVersion : uint8_t {
        AUTO = 0x00,
        IPV4 = 0x01,
        IPV6 = 0x02
    };


    /*!
     * @brief Default constructor.
     */
    GetOobBootOptions();


    /*! Copy constructor */
    GetOobBootOptions(const GetOobBootOptions&) = default;


    /*! Assignment operator */
    GetOobBootOptions& operator=(const GetOobBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) = 0;


    uint16_t get_max_length() const { return m_max_param_len; }


    uint16_t get_length() const { return m_req_param_len; }


protected:

    void set_max_length(uint16_t len) { m_max_param_len = len; }


    void set_length(uint16_t len) { m_req_param_len = len; }


private:

    uint16_t m_max_param_len{};
    uint16_t m_req_param_len{};
    uint8_t m_req_param_version{};
    uint8_t m_req_param_index{};

protected:

    static constexpr size_t IP_BYTES = 16;
    static constexpr size_t MAC_BYTES = 6;

    static constexpr auto NULL_ADDY = "0.0.0.0";
    static constexpr auto NULL_MAC = "00:00:00:00:00:00";

    static constexpr std::uint8_t PARAMETER_VERSION = 0x02;

    static constexpr size_t OFFSET_MAX_SIZE = OFFSET_COMPLETION_CODE + 1;
    static constexpr size_t OFFSET_CUR_SIZE = OFFSET_MAX_SIZE + 2;

    static constexpr size_t OFFSET_VERSION = OFFSET_CUR_SIZE + 2;
    static constexpr size_t OFFSET_INDEX = OFFSET_VERSION + 1;

    static constexpr size_t MAX_RESPONSE_SIZE = 255;
    static constexpr size_t MIN_RESPONSE_SIZE = OFFSET_INDEX + 1;


    virtual size_t v2_payload_size() = 0;


    bool valid_basic_parameter(const std::vector<uint8_t>& data);


    void set_version(uint8_t version) { m_req_param_version = version; }


    void set_index(uint8_t index) { m_req_param_index = index; }


    inline uint16_t pull_uint16(const std::vector <uint8_t>& data, size_t offset) const {
        return uint16_t(data[offset + 1] << 8 | data[offset]);
    }


    uint64_t pull_uint64(const std::vector<uint8_t>& data, size_t offset) const;


    std::string pull_ip(const std::vector<uint8_t>& data, size_t offset) const;


    std::string pull_mac(const std::vector<uint8_t>& data, size_t offset) const;


public:

    uint16_t get_index() const { return m_req_param_index; }


    uint16_t get_version() const { return m_req_param_version; }


    virtual std::string what() = 0;
};

}

}
}
}
