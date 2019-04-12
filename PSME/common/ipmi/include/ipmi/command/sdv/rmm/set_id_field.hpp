/*!
 * @brief SetIdField IPMI command request and response.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/command/sdv/rmm/set_id_field.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * SetIdField command request
 */
class SetIdField : public ipmi::Request {
public:
    /*!
     * @brief Constructors
     */
    SetIdField();
    SetIdField(const SetIdField&) = default;
    SetIdField(SetIdField&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~SetIdField();


    const char* get_command_name() const override {
        return "SetIdField";
    }


    /*!
     * @brief Set id field type that is to be set
     * @param id_field_type Id field type
     */
    void set_id_field_type(IdFieldType id_field_type) {
        m_id_field_type = id_field_type;
    }


    /*!
     * @brief Set numeric data
     * @param data Data to set
     */
    void set_data_as_number(std::uint32_t data) {
        m_numeric_data = data;
        m_is_data_numeric = true;
    }


    /*!
     * @brief Set string data to send
     * @param data String data to send
     */
    void set_data_as_string(const std::string& data) {
        m_string_data = data;
        m_is_data_numeric = false;
    }


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;


    IdFieldType m_id_field_type{IdFieldType::RESERVED};
    std::uint32_t m_numeric_data{};
    std::string m_string_data{};
    bool m_is_data_numeric{false};
};

}

namespace response {

/*!
 * SetIdField command response
 */
class SetIdField : public ipmi::Response {
public:
    /*!
     * @brief Constructor
     */
    SetIdField();
    SetIdField(const SetIdField&) = default;
    SetIdField(SetIdField&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~SetIdField();


    const char* get_command_name() const override {
        return "SetIdField";
    }


    /*!
     * @brief Get maximum data size that can be sent
     * @return Maximum data size that can be sent
     */
    std::uint8_t get_maximum_data_size() const {
        return m_maximum_data_size;
    }


private:
    static constexpr std::size_t RESPONSE_SIZE = 2;
    static constexpr size_t MAXIMUM_DATA_SIZE_OFFSET = 1;


    void unpack(const IpmiInterface::ByteBuffer& byte_buffer) override;


    std::uint8_t m_maximum_data_size{};
};

}

}
}
}
