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
 * @file set_iscsi_field.hpp
 *
 * @brief Set iSCSI Field Command Declaration.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI Set iSCSI Field.
 */
class SetIscsiField : public Request {
public:

    /*!
     * @brief Represents field instance type. Need to be specific before send write request to field.
     */
    enum class FieldInstance : std::uint8_t {
        BOOT_PRIORITY = 0x01,
        INITIATOR_NAME = 0x02,
        INITIATOR_DHCP = 0x03,
        EXPANSION_ROM_MENU = 0x04,
        CONNECTION_WAIT_TIME = 0x05,
        INITIATOR_IP = 0x06,
        INITIATOR_NETMASK = 0x07,
        GATEWAY = 0x08,
        ISCSI_VLAN = 0x09,
        TARGET_NAME = 0x0A,
        TARGET_DHCP = 0x0B,
        TARGET_IP = 0x0C,
        TARGET_PORT = 0x0D,
        BOOT_LUN = 0x0E,
        AUTH_METHOD = 0x0F,
        CHAP_USERNAME = 0x10,
        CHAP_SECRET = 0x11,
        MUTUAL_CHAP_SECRET = 0x12,
        BOOT_FLAGS = 0x13,
        NIC = 0x14,
        LAST,
        UNKNOWN,
    };

    /*!
     * @brief Default constructor.
     */
    SetIscsiField();

    /*! Copy constructor. */
    SetIscsiField(const SetIscsiField&) = default;

    /*! Assignment operator */
    SetIscsiField& operator=(const SetIscsiField&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetIscsiField();

    /*!
     * @brief Sets field instance type. Need to be done before write request.
     * @param type FieldInstance type of the field.
     */
    void set_field_instance(const FieldInstance type) {
        m_field_instance_type = type;
    }

    /*!
     * @brief Sets data to be send. Must be compatible with set FieldInstance.
     * @param data vector of data to be send.
     */
    void set_data(const std::vector<std::uint8_t>& data) {
        m_data = data;
    }

    // TODO: Added specialized methods to set specific kind of fields.

    virtual void pack(std::vector<std::uint8_t>& data) const override;

private:
    FieldInstance m_field_instance_type{FieldInstance::UNKNOWN};
    std::vector<std::uint8_t> m_data{};

    void append_vector(std::vector<std::uint8_t>& output, const std::vector<std::uint8_t>& input) const;

};

}

namespace response {

/*!
 * @brief Response message for IPMI Set iSCSI Field.
 */
class SetIscsiField: public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetIscsiField();

    /*! Copy constructor. */
    SetIscsiField(const SetIscsiField&) = default;

    /*! Assignment operator */
    SetIscsiField& operator=(const SetIscsiField&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetIscsiField();

    /*!
     * @brief Returns maximum number of data bytes possible for field.
     * @return maximum number of possible bytes for field.
     */
    std::uint8_t get_field_max_bytes() const {
        return m_field_max_byte_no;
    }

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

private:
    std::uint8_t m_field_max_byte_no{0};
    static constexpr std::size_t OFFSET_MAX_BYTES_NUMBER = 1;
    static constexpr std::size_t RESPONSE_SIZE = 2;
};

}

}
}
}
