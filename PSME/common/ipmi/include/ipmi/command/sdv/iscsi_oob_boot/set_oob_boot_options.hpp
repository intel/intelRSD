/*!
 * @brief Set OOB Boot Options command Base Class declaration.
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



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOB Boot Options.
 */
class SetOobBootOptions : public Request {
public:

    enum class ParameterType : uint8_t {
        CONTROL = 0X00,
        INITIATOR = 0X01,
        NIC = 0X02,
        TARGET = 0X03,
        HEAP = 0X04,
        RESET = 0x7f
    };

    enum class IPVersion : uint8_t {
        AUTO = 0x00,
        IPV4 = 0x01,
        IPV6 = 0x02
    };


    /*!
     * @brief Default constructor.
     */
    SetOobBootOptions(ParameterType id);


    /*! Copy constructor */
    SetOobBootOptions(const SetOobBootOptions&) = default;


    /*! Assignment operator */
    SetOobBootOptions& operator=(const SetOobBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const = 0;


    void set_params_ready(bool valid) { m_param_valid_flag = valid; }


protected:

    static constexpr auto IP_BYTES = 16;
    static constexpr auto MAC_BYTES = 6;
    static constexpr auto NULL_ADDRESS = "0.0.0.0";
    static constexpr auto NULL_MAC = "00:00:00:00:00:00";
    static constexpr std::uint8_t PARAMETER_VERSION = 0x02;


    uint8_t get_parameter_id() const;


    void push_uint16(std::vector<uint8_t>& output, uint16_t input) const;


    void push_uint32(std::vector<uint8_t>& output, uint32_t input) const;


    void push_uint64(std::vector<uint8_t>& output, uint64_t input) const;


    void push_ip(std::vector<uint8_t>& output, std::string input) const;


    void push_mac(std::vector<uint8_t>& output, std::string input) const;


private:

    uint8_t m_id{};
    bool m_param_valid_flag{false};

    static constexpr std::uint8_t PARAMETERS_VALID = 0x80;
    static constexpr std::uint8_t PARAMETERS_UNAVAILABLE = 0x00;
};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOB Boot option.
 */
class SetOobBootOptions : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobBootOptions();


    /*! Copy constructor */
    SetOobBootOptions(const SetOobBootOptions&) = default;


    /*! Assignment operator */
    SetOobBootOptions& operator=(const SetOobBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) = 0;


private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
